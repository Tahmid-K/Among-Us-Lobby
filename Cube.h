#ifndef _CUBE_H_
#define _CUBE_H_

#include "Object.h"

#include <vector>

class cube : public Object {
private:
    GLuint VAO;
    GLuint VBO, EBO;

public:
    cube(float size);
    ~cube();

    void draw(const glm::mat4& view, const glm::mat4& projection, GLuint shader) override;
    void update() override;

    void spin(float deg);
};

#endif
