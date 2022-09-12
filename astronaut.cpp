#include "astronaut.h"
#include <iostream>

#include "Window.h"


astronaut::astronaut(const std::string& obj_filename,
                   lighting the_lighting, glm::vec3 color, const glm::vec3 translation,
                   const glm::vec3 scale_factor) : lighting_(the_lighting), color_(color) {
    model_matrix_ = glm::mat4(1);
    if (const auto num_lines = parse_obj(obj_filename)) {
        std::clog << num_lines << " lines processed" << std::endl;
        find_center_and_radius();
        scale_and_translate(translation, scale_factor);
    }
    else {
        std::cerr << "unable to parse " << obj_filename << std::endl;
    }
    glGenVertexArrays(1, &vao_);
    // Bind to the VAO.
    glBindVertexArray(vao_);
    // Bind VBO to the bound VAO, and store the vertex data
    glGenBuffers(1, &vbo1_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo1_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * out_vertices_.size(), out_vertices_.data(), GL_STATIC_DRAW);
    // Enable Vertex Attribute 0 to pass the vertex data through to the shader
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

    // Bind VBO to the bound VAO, and store the vertex data
    glGenBuffers(1, &vbo2_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo2_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * out_normals_.size(), out_normals_.data(), GL_STATIC_DRAW);
    // Enable Vertex Attribute 1 to pass the vertex data through to the shader
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

    // Unbind the VBO/VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

astronaut::~astronaut() {
    // Delete the VBO/EBO and the VAO.
    glDeleteBuffers(1, &vbo1_);
    glDeleteVertexArrays(1, &vao_);
}

void astronaut::draw(const glm::mat4 current_matrix, const GLuint shader) {
    // Actiavte the shader program
    model_matrix_ = current_matrix * model_matrix_;
    glUseProgram(shader);
    // Get the shader variable locations and send the uniform data to the shader
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, false, value_ptr(window::view));
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, false, value_ptr(window::projection));
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, value_ptr(model_matrix_));
    glUniform3fv(glGetUniformLocation(shader, "view_pos"), 1, value_ptr(glm::vec3(window::eye_pos)));
    glUniform3fv(glGetUniformLocation(shader, "color"), 1, value_ptr(glm::vec3(color_)));
    lighting_.send_mat_to_shader(shader);

    // Bind the VAO
    glBindVertexArray(vao_);

    // Draw the points using triangles, indexed with the EBO
    //glDrawElements(GL_TRIANGLES, 3 * vertex_indices_.size(), GL_UNSIGNED_INT, nullptr);
    glDrawArrays(GL_TRIANGLES, 0, 3 * vertex_indices_.size());
    // Unbind the VAO and shader program
    glBindVertexArray(0);
    glUseProgram(0);
}

void astronaut::update() {
}

int astronaut::parse_obj(const std::string& file_name) {
    auto num_lines = 0;
    std::ifstream obj_file(file_name);
    // Check whether the file can be opened.
    if (obj_file.is_open()) {
        std::string line; // A line in the file.
        // Read lines from the file.
        while (std::getline(obj_file, line)) {
            // Turn the line into a string stream for processing.
            std::stringstream ss;
            ss << line;
            // Read the first word of the line.
            std::string label;
            ss >> label;
            // If the line is about vertex (starting with a "v").
            if (label == "v") {
                // Read the later three float numbers and use them as the 
                // coordinates.
                glm::vec3 point;
                ss >> point.x >> point.y >> point.z;
                vertices_.push_back(point);
                num_lines++;
            }
                // else if the line is a vertex normal (starting with a "vn")
            else if (label == "vn") {
                glm::vec3 point;
                ss >> point.x >> point.y >> point.z;
                normals_.push_back(point);
                num_lines++;
            }
                // else if the line is a face (starting with a "f")
            else if (label == "f") {
                std::vector<std::string> a_vector(3);
                ss >> a_vector[0] >> a_vector[1] >> a_vector[2];
                for (const auto& a_string : a_vector) {
                    const auto vertex_pos = a_string.find('/');
                    const auto normal_pos = a_string.find_last_of('/');
                    if (vertex_pos != std::string::npos && normal_pos != std::string::npos) {
                        vertex_indices_.push_back(std::stoi(a_string.substr(0, vertex_pos))-1);
                        normal_indices_.push_back(std::stoi(a_string.substr( normal_pos + 1))-1);
                    }
                    else {
                        std::cerr << "Can't open the file " << file_name << std::endl;
                    }

                }
                num_lines++;
            }
        }
    }
    else {
        std::cerr << "Can't open the file " << file_name << std::endl;
    }
    obj_file.close();
    reorder();
    return num_lines;
}

void astronaut::scale_and_translate(const glm::vec3 translation, const glm::vec3 scale_factor) {
    // maybe calculate to seperate matricies?
    model_matrix_ = scale(model_matrix_, scale_factor);
    model_matrix_ = translate(model_matrix_, translation);    
}

void astronaut::reorder() {
    for (auto vertex_index : vertex_indices_) {
        auto& vertex = vertices_[vertex_index];
        out_vertices_.push_back(vertex);
    }

    for (auto normal_index : normal_indices_) {
        auto& normal = normals_[normal_index];
        out_normals_.push_back(normal);
    }
}

void astronaut::find_center_and_radius() {
    auto max_x = FLT_MIN, max_y = FLT_MIN, max_z = FLT_MIN;
    auto min_x = FLT_MAX, min_y = FLT_MAX, min_z = FLT_MAX;
    for(const auto &vertex : vertices_) {
        max_x = std::max(max_x, vertex.x);
        max_y = std::max(max_y, vertex.y);
        max_z = std::max(max_z, vertex.z);

        min_x = std::min(min_x, vertex.x);
        min_y = std::min(min_y, vertex.y);
        min_z = std::min(min_z, vertex.z);
    }
    const auto center_x = (max_x + min_x)/2;
    const auto center_y = (max_y + min_y)/2;
    const auto center_z = (max_z + min_z)/2;

    center_point = glm::vec3(center_x, center_y, center_z);
    const auto radius_x = std::abs(max_x - min_x)/2;
    const auto radius_y = std::abs(max_y - min_y)/2;
    const auto radius_z = std::abs(max_z - min_z)/2;
    const auto maximums = std::vector<float>({radius_x, radius_y, radius_z});
    radius = *std::max_element(maximums.begin(), maximums.end());
}
