#ifndef _MATERIALS_H_
#define _MATERIALS_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class materials {
private:
    glm::vec3 ambient_, diffuse_, specular_;
    float shininess_;
public:
    materials(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess);
    void send_mat_to_shader(const int shader_id);
};
#endif
