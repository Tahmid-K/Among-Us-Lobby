#include "PointCloud.h"




point_cloud::point_cloud(std::string objFilename, GLfloat point_size)
    : pointSize(point_size) {
    /*
     * TODO: Section 2: Currently, all the points are hard coded below.
     * Modify this to read points from an obj file.
     */
    std::clog << "parsing file: " << objFilename << std::endl;
    if(const auto num_lines = parse_obj(objFilename)) {
        std::clog << num_lines << " lines processed" << std::endl;
    }
    else {
        points_ = {
            glm::vec3(-2.5, 2.5, 2.5),
            glm::vec3(-2.5, -2.5, 2.5),
            glm::vec3(2.5, -2.5, 2.5),
            glm::vec3(2.5, 2.5, 2.5),
            glm::vec3(-2.5, 2.5, -2.5),
            glm::vec3(-2.5, -2.5, -2.5),
            glm::vec3(2.5, -2.5, -2.5),
            glm::vec3(2.5, 2.5, -2.5)
        };
    }

    /*
     * TODO: Section 4, you will need to normalize the object to fit in the
     * screen.
     */
    
    auto max_x = FLT_MIN, max_y = FLT_MIN, max_z = FLT_MIN;
    auto min_x = FLT_MAX, min_y = FLT_MAX, min_z = FLT_MAX;
    for(const auto &point : points_) {
        max_x = std::max(max_x, point.x);
        max_y = std::max(max_y, point.y);
        max_z = std::max(max_z, point.z);

        min_x = std::min(min_x, point.x);
        min_y = std::min(min_y, point.y);
        min_z = std::min(min_z, point.z);
    }
    const auto center_x = (max_x + min_x)/2;
    const auto center_y = (max_y + min_y)/2;
    const auto center_z = (max_z + min_z)/2;

    for(auto &point : points_) {
        point.x -= center_x;
        point.y -= center_y;
        point.z -= center_z;
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
    
    for(auto& point : points_) {
        point.x *= scale;
        point.y *= scale;
        point.z *= scale;
    }
   
    // Set the model matrix to an identity matrix.
    model = glm::mat4(1);

    // Set the color.
    color = glm::vec3(1, 0, 0);

    // Generate a Vertex Array (VAO) and Vertex Buffer Object (VBO)
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind VAO
    glBindVertexArray(VAO);

    // Bind VBO to the bound VAO, and store the point data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points_.size(), points_.data(), GL_STATIC_DRAW);
    // Enable Vertex Attribute 0 to pass point data through to the shader
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

    // Unbind the VBO/VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

point_cloud::~point_cloud() {
    // Delete the VBO and the VAO.
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void point_cloud::draw(const glm::mat4& view, const glm::mat4& projection, GLuint shader) {
    // Actiavte the shader program
    glUseProgram(shader);

    // Get the shader variable locations and send the uniform data to the shader
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, false, value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, false, value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, value_ptr(model));
    glUniform3fv(glGetUniformLocation(shader, "color"), 1, value_ptr(color));

    // Bind the VAO
    glBindVertexArray(VAO);

    // Set point size
    glPointSize(pointSize);

    // Draw the points
    glDrawArrays(GL_POINTS, 0, points_.size());

    // Unbind the VAO and shader program
    glBindVertexArray(0);
    glUseProgram(0);
}

void point_cloud::update() {
    // Spin the cube by 1 degree
    spin(0.1f);
}

void point_cloud::update_point_size(GLfloat size) {
    /*
     * TODO: Section 3: Implement this function to adjust the point size.
     */
    if(size > 0) {
        pointSize*=size;
    }
    else {
        pointSize/= -size;
    }
    std::clog << "point size is now: " << pointSize << std::endl;

}

void point_cloud::spin(float deg) {
    // Update the model matrix by multiplying a rotation matrix
    model = model * glm::rotate(glm::radians(deg), glm::vec3(0.0f, 1.0f, 0.0f));
}

int point_cloud::parse_obj(const std::string& file_name) {
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
                points_.push_back(point);
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
