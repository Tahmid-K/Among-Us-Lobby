#include "Transform.h"

transform::transform(const glm::mat4 a_matrix) : model_matrix_(a_matrix) {
}

transform::~transform() {
    for(auto* a_node : node_list_) {
        delete a_node;
    }
}

void transform::swap_transform(glm::mat4 a_matrix) {
    model_matrix_ = a_matrix;
}

void transform::add_node(node* a_node) {
    node_list_.push_back(a_node);
}

void transform::draw(const glm::mat4 current_matrix, const GLuint shader) {
    const auto new_matrix = current_matrix * model_matrix_;
    for(auto* a_node : node_list_) {
        a_node->draw(new_matrix, shader);
    }
}

void transform::update() {
    /*const auto degree = .1f;
    const auto a_matrix = glm::rotate(glm::radians(degree), glm::vec3(0.0f, 1.0f, 0.0f);
    model_matrix_ = model_matrix_ * glm::rotate(glm::radians(degree), glm::vec3(0.0f, 1.0f, 0.0f));*/
}
