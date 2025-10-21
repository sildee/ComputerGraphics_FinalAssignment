#pragma once
#include "mesh.h"
class Cube : public Mesh {
private:

public:
    std::vector<GLushort> cube_elements;
	Cube(const string& texture, float x = 1, float y = 1, float z = 1);
	void render(glm::mat4 matrix, GLuint program_id, glm::mat4 view, glm::mat4 projection, LightSources lights) override;
	void initMesh() override;
	Cube* clone() override;
	Cube(const Cube& other);
};