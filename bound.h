#pragma once
#include <glm/vec3.hpp>

class bound {
public:
    bound(glm::vec3 a_center_point, float a_radius);
    bound() = default;
    glm::vec3 center_point;
    float radius;

};

