#include "plane.h"

plane::plane(glm::vec3 normal, glm::vec3 point_on_plane) : normal_(normal), point_(point_on_plane) {
}

float plane::shortest_distance(glm::vec3 point) {
    return glm::dot((point - point_), normal_);
}
