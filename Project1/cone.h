#pragma once
#include "mesh.h"

class Cone : public Mesh {
public:
    Cone(const std::string& texture, int segments = 32, float radius = 1.0f, float height = 2.0f);
    void initMesh() override;
    void render(glm::mat4 matrix, GLuint program_id, glm::mat4 view, glm::mat4 projection, LightSources lights) override;
	Cone* clone() override;
	Cone(const Cone& other);
private:
    std::vector<GLushort> cone_elements;
};
