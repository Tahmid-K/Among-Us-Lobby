#include "skybox.h"

skybox::skybox(GLfloat point_size) {
    // Generate a vertex array (VAO) and vertex buffer object (VBO).
    model = glm::mat4(1);
    // Set the color.
    color = glm::vec3(1, 0, 0);
    //window::view = camera.GetViewMatrix();
    const std::vector<std::string> faces {
        "./skybox/right.jpg",
        "./skybox/left.jpg",
        "./skybox/top.jpg",
        "./skybox/bottom.jpg",
        "./skybox/front.jpg",
        "./skybox/back.jpg"
    };
    cubemap_texture_ = load_skybox(faces);
GLfloat skybox_vertices[] = {
        -500.0f,  500.0f, -500.0f,
        -500.0f, -500.0f, -500.0f,
        500.0f, -500.0f, -500.0f,
        500.0f, -500.0f, -500.0f,
        500.0f,  500.0f, -500.0f,
        -500.0f,  500.0f, -500.0f,
        
        -500.0f, -500.0f,  500.0f,
        -500.0f, -500.0f, -500.0f,
        -500.0f,  500.0f, -500.0f,
        -500.0f,  500.0f, -500.0f,
        -500.0f,  500.0f,  500.0f,
        -500.0f, -500.0f,  500.0f,
        
        500.0f, -500.0f, -500.0f,
        500.0f, -500.0f,  500.0f,
        500.0f,  500.0f,  500.0f,
        500.0f,  500.0f,  500.0f,
        500.0f,  500.0f, -500.0f,
        500.0f, -500.0f, -500.0f,
        
        -500.0f, -500.0f,  500.0f,
        -500.0f,  500.0f,  500.0f,
        500.0f,  500.0f,  500.0f,
        500.0f,  500.0f,  500.0f,
        500.0f, -500.0f,  500.0f,
        -500.0f, -500.0f,  500.0f,
        
        -500.0f,  500.0f, -500.0f,
        500.0f,  500.0f, -500.0f,
        500.0f,  500.0f,  500.0f,
        500.0f,  500.0f,  500.0f,
        -500.0f,  500.0f,  500.0f,
        -500.0f,  500.0f, -500.0f,
        
        -500.0f, -500.0f, -500.0f,
        -500.0f, -500.0f,  500.0f,
        500.0f, -500.0f, -500.0f,
        500.0f, -500.0f, -500.0f,
        -500.0f, -500.0f,  500.0f,
        500.0f, -500.0f,  500.0f
    };
    glGenVertexArrays(1, &skybox_vao_);
    glGenBuffers(1, &skybox_vbo_);
    glBindVertexArray(skybox_vao_);
    glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

unsigned skybox::load_skybox(std::vector<std::string> faces) {
    unsigned int texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    int width, height, nr_channels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nr_channels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texture_id;
}

void skybox::draw(const glm::mat4& view, const glm::mat4& projection, GLuint shader) {
    // Actiavte the shader program
    glUseProgram(shader); // 

    glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
    //window::view = glm::mat4(glm::mat3(window::eye_pos.GetViewMatrix())); // remove translation from the view matrix
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, false, value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, false, value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, value_ptr(model));

    // skybox cube
    glBindVertexArray(skybox_vao_);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture_);
    //
    // Make sure no bytes are padded:
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Use bilinear interpolation:
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Use clamp to edge to hide skybox edges:
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //
    glDrawArrays(GL_TRIANGLES, 0, 36); // change count
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // set depth function back to default

    // Unbind the VAO and shader program
    glBindVertexArray(0);
    glUseProgram(0);
}

void skybox::update() {
}

void skybox::set_model(const glm::mat4 a_model) {
    model = a_model;
}