#include "cylinder.h"
#include <vector>

//Cylinder normals/vertices/uvs made with assistance from https://www.mbsoftworks.sk/tutorials/opengl4/022-cylinder-and-sphere/
//and then tweaked based on lessons learned from the Cone implementation
Cylinder::Cylinder(const std::string& texture, int segments, float radius, float height) : Mesh() {
    setDefaultMaterial();
    texture_id = loadBMP(texture.c_str());
    usesTexture = 1;

    const float angleStep = 2.0f * glm::pi<float>() / float(segments);
    float currentSliceAngle = 0.0f;
    float halfHeight = height / 2.0f;

    for (int i = 0; i <= segments; ++i) {
        float angle = i * angleStep;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        glm::vec3 normal = glm::normalize(glm::vec3(x, 0.0f, z));

        vertices.push_back(glm::vec3(x, -halfHeight, z));
        normals.push_back(normal);
        uvs.push_back(glm::vec2((float)i / segments, 0.0f));

        vertices.push_back(glm::vec3(x, halfHeight, z));
        normals.push_back(normal);
        uvs.push_back(glm::vec2((float)i / segments, 1.0f));
    }

    for (int i = 0; i < segments; ++i) {
        int idx = i * 2;
        cylinder_elements.push_back(idx);
        cylinder_elements.push_back(idx + 1);
        cylinder_elements.push_back(idx + 3);

        cylinder_elements.push_back(idx);
        cylinder_elements.push_back(idx + 3);
        cylinder_elements.push_back(idx + 2);
    }

    GLuint bottom_center_idx = vertices.size();
    vertices.push_back(glm::vec3(0.0f, -halfHeight, 0.0f));
    normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
    uvs.push_back(glm::vec2(0.5f, 0.5f));

    GLuint top_center_idx = vertices.size();
    vertices.push_back(glm::vec3(0.0f, halfHeight, 0.0f));
    normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    uvs.push_back(glm::vec2(0.5f, 0.5f));

    // Bottom cap
    for (int i = 0; i < segments; ++i) {
        int idx = i * 2;
        cylinder_elements.push_back(bottom_center_idx);
        cylinder_elements.push_back(idx);
        cylinder_elements.push_back((idx + 2) % (segments * 2));
    }

    // Top cap
    for (int i = 0; i < segments; ++i) {
        int idx = i * 2 + 1;
        cylinder_elements.push_back(top_center_idx);
        cylinder_elements.push_back((idx + 2) % (segments * 2));
        cylinder_elements.push_back(idx);
    }
    initMesh();
}

void Cylinder::initMesh() {
    GLuint vbo_vertices, vbo_normals, vbo_uvs, ibo_indices;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vbo_normals);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &vbo_uvs);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_uvs);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), uvs.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &ibo_indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cylinder_elements.size() * sizeof(GLushort), cylinder_elements.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void Cylinder::render(glm::mat4 matrix, GLuint program_id, glm::mat4 view, glm::mat4 projection, LightSources lights) {
    glm::mat4 mv = view * matrix;

    GLuint uniform_mv = glGetUniformLocation(program_id, "mv");
    GLuint uniform_proj = glGetUniformLocation(program_id, "projection");
    GLuint uniform_light_pos = glGetUniformLocation(program_id, "light_pos");
    GLuint uniform_material_ambient = glGetUniformLocation(program_id, "mat_ambient");
    GLuint uniform_material_diffuse = glGetUniformLocation(program_id, "mat_diffuse");
    GLuint uniform_specular = glGetUniformLocation(program_id, "mat_specular");
    GLuint uniform_material_power = glGetUniformLocation(program_id, "mat_power");
    GLuint uniform_uses_texture = glGetUniformLocation(program_id, "model_uses_texture");

    glUseProgram(program_id);
    glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(mv));
    glUniformMatrix4fv(uniform_proj, 1, GL_FALSE, glm::value_ptr(projection));
    handleLights(program_id, lights);
    glUniform3fv(uniform_material_ambient, 1, glm::value_ptr(material.ambient_color));
    glUniform3fv(uniform_material_diffuse, 1, glm::value_ptr(material.diffuse_color));
    glUniform3fv(uniform_specular, 1, glm::value_ptr(material.specular));
    glUniform1f(uniform_material_power, material.power);
    glUniform1i(uniform_uses_texture, usesTexture);

    if (usesTexture)
        glBindTexture(GL_TEXTURE_2D, texture_id);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, cylinder_elements.size(), GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
}
Cylinder::Cylinder(const Cylinder& other) {
    this->vertices = other.vertices;
    this->normals = other.normals;
    this->uvs = other.uvs;
    this->cylinder_elements = other.cylinder_elements;
    this->material = other.material;
    this->usesTexture = other.usesTexture;

    if (this->usesTexture) {
        this->texture_id = other.texture_id;
    }

    this->initMesh();
}
Cylinder* Cylinder::clone() {
    return new Cylinder(*this);
}