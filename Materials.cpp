#include "Materials.h"

materials::materials(const glm::vec3 ambient, const glm::vec3 diffuse, const glm::vec3 specular, const float shininess)
    : ambient_(ambient), diffuse_(diffuse), specular_(specular), shininess_(shininess) {
}

void materials::send_mat_to_shader(const int shader_id) {
    glUniform3fv(glGetUniformLocation(shader_id, "a_material.ambient"), 1, value_ptr(ambient_));
    glUniform3fv(glGetUniformLocation(shader_id, "a_material.diffuse"), 1, value_ptr(diffuse_));
    glUniform3fv(glGetUniformLocation(shader_id, "a_material.specular"), 1, value_ptr(specular_));
    glUniform1f(glGetUniformLocation(shader_id, "a_material.shininess"), shininess_);
}
