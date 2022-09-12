#ifndef _LIGHTING_H_
#define _LIGHTING_H_
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class lighting {
    private:
    glm::vec3 ambient_, diffuse_, specular_, position_, attenuation_;
public:
    lighting(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 position, glm::vec3 attenuation);
    glm::vec3 get_position() const;
    void set_position(glm::vec3 a_position);
    void send_mat_to_shader(const int shader_id);
};
#endif