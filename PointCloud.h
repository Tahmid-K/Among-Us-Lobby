#ifndef _POINT_CLOUD_H_
#define _POINT_CLOUD_H_

#include "Object.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

class point_cloud : public Object {
private:
    std::vector<glm::vec3> points_;

    GLuint VAO, VBO;
    GLfloat pointSize;

public:
    point_cloud(std::string objFilename, GLfloat point_size);
    ~point_cloud();

    void draw(const glm::mat4& view, const glm::mat4& projection, GLuint shader) override;
    void update() override;

    void update_point_size(GLfloat size);
    void spin(float deg);

protected:
    int parse_obj(const std::string& file_name);
};

#endif
