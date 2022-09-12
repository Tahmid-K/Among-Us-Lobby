#pragma once
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
class plane {
public:
    plane(glm::vec3 normal, glm::vec3 point_on_plane);
    float shortest_distance(glm::vec3 point);
private:
    glm::vec3 normal_;
    glm::vec3 point_;

};

