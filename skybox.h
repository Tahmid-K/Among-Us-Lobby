#ifndef _SKYBOX_H_
#define _SKYBOX_H_
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include "Lighting.h"
#include "Materials.h"
#include "Object.h"
#include "stb_image.h"
#include "Window.h"

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class skybox : Object {
private:
    std::vector<glm::vec3> vertices_;
    std::vector<glm::vec3> normals_;
    std::vector<glm::ivec3> indices_;

    GLuint vao_, vbo1_, vbo2_, ebo_, skybox_vao_, skybox_vbo_, cubemap_texture_;
    GLfloat point_size_;

    //lighting lighting_;
    //materials materials_;

    bool light_source_;

    GLint color_switch_;

    glm::vec3 light_position_;
    glm::vec3 center_point_;

public:
    skybox(GLfloat point_size);
    unsigned int load_skybox(std::vector<std::string> faces);
    void draw(const glm::mat4& view, const glm::mat4& projection, GLuint shader) override;
    void update() override;
    void set_model(const glm::mat4 a_model);
};
#endif
