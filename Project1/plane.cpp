#include "plane.h"

Plane::Plane(const string& texture, const float x, const float z) : Mesh() {
    setDefaultMaterial();
    this->texture_id = loadBMP(texture.c_str());
    usesTexture = 1;

    vertices = {
        {-x, 0.0f, -z}, {x, 0.0f, -z}, {x, 0.0f, z},
        {-x, 0.0f, -z}, {x, 0.0f, z}, {-x, 0.0f, z}
    };

    normals = {
        {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}
    };
    uvs = {
        {0.0f, 0.0f}, {10.0f, 0.0f}, {10.0f, 10.0f},
        {0.0f, 0.0f}, {10.0f, 10.0f}, {0.0f, 10.0f}
    };

    initMesh();
}