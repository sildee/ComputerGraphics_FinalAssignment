#pragma once
#include <glm/gtc/type_ptr.hpp>
struct LightSource
{
	glm::vec3 position;
	glm::vec3 color;
	float intensity;
};
using LightSources = std::vector<LightSource>;