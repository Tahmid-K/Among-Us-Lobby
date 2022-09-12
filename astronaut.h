#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "bound.h"
#include "Lighting.h"
#include "Materials.h"
#include "Object.h"
#include "node.h"

class astronaut : public node, public bound {
    public:
    astronaut(const std::string& obj_filename, lighting the_lighting, glm::vec3 color, glm::vec3 translation,
             glm::vec3 scale_factor);
    ~astronaut() override;
    void draw(glm::mat4 current_matrix, const GLuint shader) override;
    void update() override;
    void handle_collision(astronaut* other_astronaut);
    void handle_collision();
    glm::mat4 get_model() const {return model_matrix_;}
    void set_model(const glm::mat4 model_matrix) {model_matrix_ = model_matrix;}

protected:
    lighting lighting_;
    glm::vec3 color_;
    GLuint vao_, vbo1_, vbo2_, ebo_;
    glm::mat4 model_matrix_;
    std::vector<glm::vec3> vertices_;
    std::vector<glm::vec3> normals_;
    std::vector<unsigned int> vertex_indices_;
    std::vector<unsigned int> normal_indices_;
    std::vector<glm::vec3> out_vertices_;
    std::vector<glm::vec3> out_normals_;
    int parse_obj(const std::string& file_name);
    void scale_and_translate(glm::vec3 translation, glm::vec3 scale_factor);
    void reorder();
    void find_center_and_radius();
};

