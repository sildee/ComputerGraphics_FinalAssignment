#pragma once
#include "mesh.h"

class Cylinder : public Mesh {
public:
    Cylinder(const std::string& texture, int segments = 32, float radius = 1.0f, float height = 2.0f);
    void initMesh() override;
    void render(glm::mat4 matrix, GLuint program_id, glm::mat4 view, glm::mat4 projection, LightSources lights) override;
    Cylinder(const Cylinder& other);
	Cylinder* clone() override;
private:
    std::vector<GLushort> cylinder_elements;
};
