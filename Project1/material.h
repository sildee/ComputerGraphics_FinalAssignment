#pragma once
#include <glm/glm.hpp>
struct Material {
    glm::vec3 ambient_color;
    glm::vec3 diffuse_color;
    glm::vec3 specular;
    float power;
};