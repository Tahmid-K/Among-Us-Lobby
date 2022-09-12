#include "Triangle.h"

triangle::triangle(const std::string& obj_filename, GLfloat point_size, materials the_materials, lighting the_lighting,
    bool light_source, GLint color_switch) :
    point_size_(point_size), lighting_(the_lighting), materials_(the_materials), light_source_(light_source), color_switch_(color_switch),
    light_position_(lighting_.get_position()), center_point_(glm::vec3(0, 0, 0)) {

    // Generate a vertex array (VAO) and vertex buffer object (VBO).
    model = glm::mat4(1);
    // Set the color.
    color = glm::vec3(1, 0, 0);

    std::clog << "parsing file: " << obj_filename << std::endl;
    if(const auto num_lines = parse_obj(obj_filename)) {
        std::clog << num_lines << " lines processed" << std::endl;
        if(!light_source_) {
            center_and_scale();
        }
        else {
            scale_down();
        }
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices_.size(), vertices_.data(), GL_STATIC_DRAW);
    // Enable Vertex Attribute 0 to pass the vertex data through to the shader
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr); 


    // Bind VBO to the bound VAO, and store the vertex data
    glGenBuffers(1, &vbo2_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo2_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals_.size(), normals_.data(), GL_STATIC_DRAW);
    // Enable Vertex Attribute 1 to pass the vertex data through to the shader
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

    // Generate EBO, bind the EBO to the bound VAO, and send the index data
    glGenBuffers(1, &ebo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::ivec3) * vertex_indices_.size(), vertex_indices_.data(), GL_STATIC_DRAW);

    // Unbind the VBO/VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);
}

triangle::~triangle() {
    // Delete the VBO/EBO and the VAO.
    glDeleteBuffers(1, &vbo1_);
    glDeleteBuffers(1, &vbo2_);
    glDeleteBuffers(1, &ebo_);
    glDeleteVertexArrays(1, &vao_);
}

void triangle::draw(const glm::mat4& view, const glm::mat4& projection, const GLuint shader) {
    // Actiavte the shader program
    glUseProgram(shader);

    // Get the shader variable locations and send the uniform data to the shader
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, false, value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, false, value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, value_ptr(model));
    glUniform3fv(glGetUniformLocation(shader, "view_pos"), 1, value_ptr(glm::vec3(0, 0, 20)));
    glUniform1i(glGetUniformLocation(shader, "color_switch"), color_switch_);
    materials_.send_mat_to_shader(shader);
    lighting_.send_mat_to_shader(shader);

    // Bind the VAO
    glBindVertexArray(vao_);

    // Draw the points using triangles, indexed with the EBO
    glDrawElements(GL_TRIANGLES, 3 * vertex_indices_.size(), GL_UNSIGNED_INT, nullptr);

    // Unbind the VAO and shader program
    glBindVertexArray(0);
    glUseProgram(0);
}

void triangle::update() {
    //spin(0.1f);
}

void triangle::switch_color() {
    color_switch_ = color_switch_ ? 0 : 1;
}

void triangle::update_point_size(const GLfloat size) {
    if(size > 0) {
        point_size_*=size;
    }
    else {
        point_size_/= -size;
    }
    std::clog << "point size is now: " << point_size_ << std::endl;
}

void triangle::spin(const float deg) {
    // Update the model matrix by multiplying a rotation matrix
    model = model * glm::rotate(glm::radians(deg), glm::vec3(0.0f, 1.0f, 0.0f));
}

void triangle::set_model(const glm::mat4 a_model) {
    model = a_model;
}

void triangle::set_materials(const glm::vec3 ambient, const glm::vec3 diffuse, const glm::vec3 specular, const float shininess) {
    materials_ = materials(ambient, diffuse, specular, shininess);
}

glm::vec3 triangle::get_light_position() const {
    return lighting_.get_position();
}

glm::vec3 triangle::get_center_point() const {
    return center_point_;
}

void triangle::set_light(const glm::vec3 ambient, const glm::vec3 diffuse, const glm::vec3 specular, const glm::vec3 position, const glm::vec3 attenuation) {
    lighting_ = lighting(ambient, diffuse, specular, position, attenuation);
}

void triangle::set_light_position(glm::vec3 a_position) {
    lighting_.set_position(a_position);
}


int triangle::parse_obj(const std::string& file_name) {
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
            else if(label == "vn") {
                glm::vec3 point;
                ss >> point.x >> point.y >> point.z;
                normals_.push_back(point);
                num_lines++;
            }
            // else if the line is a face (starting with a "f")
            else if(label == "f") {
                glm::ivec3 an_index;
                std::vector<std::string> a_vector(3);
                std::vector<int> temp_index;
                ss >> a_vector[0] >> a_vector[1] >> a_vector[2];
                for(const auto& a_string : a_vector) {
                    const auto vertex_pos = a_string.find('/');
                    const auto normal_pos = a_string.find_last_of('/');
                    if(vertex_pos != std::string::npos) {
                        temp_index.push_back(std::stoi(a_string.substr(vertex_pos+2))-1);
                    }
                }
                an_index.x = temp_index[0];
                an_index.y = temp_index[1];
                an_index.z = temp_index[2];
                vertex_indices_.push_back(an_index);
                num_lines++;
            }
        }
    }
    else {
        std::cerr << "Can't open the file " << file_name << std::endl;
    }
    obj_file.close();
    return num_lines;
}

void triangle::center_and_scale() {
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

    center_point_ = glm::vec3(center_x, center_y, center_z);

    for(auto &vertex : vertices_) {
        vertex.x -= center_x;
        vertex.y -= center_y;
        vertex.z -= center_z;
    }

    max_x -= center_x;
    max_y -= center_y;
    max_z -= center_z;
    min_x -= center_x;
    min_y -= center_y;
    min_z -= center_z;

    const auto maximums = std::vector<float>({max_x, max_y, max_z, std::abs(min_x), std::abs(min_y), std::abs(min_z)});
    auto scale = *std::max_element(maximums.begin(), maximums.end());
    scale = 7/scale;
    
    for(auto& vertex : vertices_) {
        vertex.x *= scale;
        vertex.y *= scale;
        vertex.z *= scale;
    }
}

void triangle::scale_down() {
    model = glm::translate(model, glm::vec3(10.0f, 0 ,0));
    model = glm::scale(model, glm::vec3(.25f, .25f, .25f));
}
