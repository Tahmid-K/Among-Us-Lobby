#include <iostream>
#include <fstream>
#include <map>
#include <tuple>
#include <numeric>

#include "Sphere.h"

#include <vector>

/* Code sampled from https://gist.github.com/zwzmzd/0195733fa1210346b00d, adjusted to use GL_QUADS instead of GL_QUADS_STRIP */
sphere::sphere(glm::vec3 translation, glm::vec3 scale_factor) {
    int i, j;
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    std::vector<GLfloat> normals;
    int indicator = 0;
    model = glm::mat4(1);
    for (i = 0; i <= stack_count; i++) {
        double lat0 = glm::pi<double>() * (-0.5 + (double)(i - 1) / stack_count);
        double z0 = sin(lat0);
        double zr0 = cos(lat0);

        double lat1 = glm::pi<double>() * (-0.5 + (double)i / stack_count);
        double z1 = sin(lat1);
        double zr1 = cos(lat1);
        for (j = 0; j <= sector_count; j++) {
            double lng = 2 * glm::pi<double>() * (double)(j - 1) / sector_count;
            double lng1 = 2 * glm::pi<double>() * (double)(j) / sector_count;

            double x1 = cos(lng1);
            double y1 = sin(lng1);
            double x = cos(lng);
            double y = sin(lng);

            vertices.push_back(x * zr1);
            vertices.push_back(y * zr1);
            vertices.push_back(z1);
            indices.push_back(indicator);
            indicator++;

            vertices.push_back(x * zr0);
            vertices.push_back(y * zr0);
            vertices.push_back(z0);
            indices.push_back(indicator);
            indicator++;

            vertices.push_back(x1 * zr0);
            vertices.push_back(y1 * zr0);
            vertices.push_back(z0);
            indices.push_back(indicator);
            indicator++;

            vertices.push_back(x1 * zr1);
            vertices.push_back(y1 * zr1);
            vertices.push_back(z1);
            indices.push_back(indicator);
            indicator++;

            float x_avg = ((x * zr1) + (x * zr0) + (x1 * zr0) + (x1 * zr1)) / 4;
            float y_avg = ((y * zr1) + (y * zr0) + (y1 * zr0) + (y1 * zr1)) / 4;
            float z_avg = (2 * z0 + 2 * z1) / 4;

            glm::vec3 avg = glm::normalize(glm::vec3(x_avg, y_avg, z_avg));

            normals.push_back(avg.x);
            normals.push_back(avg.y);
            normals.push_back(avg.z);

            normals.push_back(avg.x);
            normals.push_back(avg.y);
            normals.push_back(avg.z);

            normals.push_back(avg.x);
            normals.push_back(avg.y);
            normals.push_back(avg.z);

            normals.push_back(avg.x);
            normals.push_back(avg.y);
            normals.push_back(avg.z);
        }
        indices.push_back(GL_PRIMITIVE_RESTART_FIXED_INDEX);
    }


    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glActiveTexture(GL_TEXTURE0);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);


    glGenBuffers(1, &vbo_n);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_n);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), &normals[0], GL_STATIC_DRAW);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    nums_to_draw = indices.size();
    scale_and_translate(translation, scale_factor);
}

void sphere::
draw(const glm::mat4& view , const glm::mat4& projection, const GLuint shader ) {
    glUseProgram(shader);

    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, false, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, false, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(vao);
    
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(GL_PRIMITIVE_RESTART_FIXED_INDEX);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glDrawElements(GL_QUADS, nums_to_draw, GL_UNSIGNED_INT, NULL);
    
    glUseProgram(0);
    glBindVertexArray(0);
}

glm::mat4 sphere::get_model() {
    return model;
}

void sphere::set_model(glm::mat4 a_model) {
    model = a_model;
}

void sphere::scale_and_translate(const glm::vec3 translation, const glm::vec3 scale_factor) {
    model = scale(model, scale_factor);
    model = translate(model, translation);   
}