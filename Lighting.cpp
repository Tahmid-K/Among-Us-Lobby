#include "Lighting.h"

lighting::lighting(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 position, glm::vec3 attenuation)
    : ambient_(ambient), diffuse_(diffuse), specular_(specular), position_(position), attenuation_(attenuation) {
}

glm::vec3 lighting::get_position() const {
    return position_;
}

void lighting::set_position(const glm::vec3 a_position) {
    position_ = a_position;
}


void lighting::send_mat_to_shader(const int shader_id) {
    glUniform3fv(glGetUniformLocation(shader_id, "a_light.ambient"), 1, value_ptr(ambient_));
    glUniform3fv(glGetUniformLocation(shader_id, "a_light.diffuse"), 1, value_ptr(diffuse_));
    glUniform3fv(glGetUniformLocation(shader_id, "a_light.specular"), 1, value_ptr(specular_));
    glUniform3fv(glGetUniformLocation(shader_id, "a_light.position"), 1, value_ptr(position_));
    glUniform3fv(glGetUniformLocation(shader_id, "a_light.attenuation"), 1, value_ptr(attenuation_));
}
