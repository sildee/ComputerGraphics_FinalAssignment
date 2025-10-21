#pragma once
#include "mesh.h"

class Pyramid : public Mesh {
public:
    Pyramid(const std::string& texture);
    void initMesh() override;
    void render(glm::mat4 matrix, GLuint program_id, glm::mat4 view, glm::mat4 projection, LightSources lights) override;
    Pyramid(const Pyramid& other);
    Pyramid* clone() override;
private:
    std::vector<GLushort> pyramid_elements;
};
