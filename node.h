#ifndef _NODE_H_
#define _NODE_H_
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
class node {
public:
    virtual ~node() = default;
    virtual void draw(glm::mat4 current_matrix, const GLuint shader) = 0;
    virtual void update() = 0;
};
#endif

