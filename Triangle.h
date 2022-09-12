#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_
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


class triangle : public Object {
private:
    std::vector<glm::vec3> vertices_;
    std::vector<glm::vec3> normals_;
    std::vector<glm::ivec3> vertex_indices_;

    GLuint vao_, vbo1_, vbo2_, ebo_;
    GLfloat point_size_;

    lighting lighting_;
    materials materials_;

    bool light_source_;

    GLint color_switch_;

    glm::vec3 light_position_;
    glm::vec3 center_point_;

    

public:
    triangle(const std::string& obj_filename, GLfloat point_size, materials the_materials, lighting the_lighting, 
        bool light_source = false, GLint color_switch = 1);
    ~triangle();

    void draw(const glm::mat4& view, const glm::mat4& projection, GLuint shader) override;
    void update() override;

    void update_point_size(GLfloat size);
    void spin(float deg);
    void switch_color();

    void set_model(glm::mat4 a_model);
    void set_light(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 position, glm::vec3 attenuation);
    void set_materials(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess);

    glm::vec3 get_light_position() const;
    glm::vec3 get_center_point() const;

    void set_light_position(glm::vec3 a_position);

protected:
    int parse_obj(const std::string& file_name);
    void center_and_scale();
    void scale_down();
};
#endif
