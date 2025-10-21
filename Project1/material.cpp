#include "material.h"
#include <glm/detail/type_vec.hpp>

Material::Material(const glm::vec3 ambient, const glm::vec3 diffuse) {
	ambient_color = ambient;
	diffuse_color = diffuse;
}