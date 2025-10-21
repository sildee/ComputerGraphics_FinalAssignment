#include "pyramid.h"

Pyramid::Pyramid(const std::string& texture) : Mesh() {
    setDefaultMaterial();
    usesTexture = 1;

    vertices = {
        // Base
        glm::vec3(-1.0f, -1.0f, -1.0f),
        glm::vec3(1.0f, -1.0f, -1.0f), 
        glm::vec3(1.0f, -1.0f, 1.0f), 
        glm::vec3(-1.0f, -1.0f, 1.0f), 

        // Front
        glm::vec3(-1.0f, -1.0f, 1.0f),
        glm::vec3(1.0f, -1.0f, 1.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),

        // Right
        glm::vec3(1.0f, -1.0f, 1.0f),
        glm::vec3(1.0f, -1.0f, -1.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),

        // Back
        glm::vec3(1.0f, -1.0f, -1.0f),
        glm::vec3(-1.0f, -1.0f, -1.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),

        // Left
        glm::vec3(-1.0f, -1.0f, -1.0f),
        glm::vec3(-1.0f, -1.0f, 1.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
    };


    normals = {
        // Base
        glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),

        // Front
        glm::vec3(0.0f, 0.707f, 0.707f), glm::vec3(0.0f, 0.707f, 0.707f), glm::vec3(0.0f, 0.707f, 0.707f),

        // Right
        glm::vec3(0.707f, 0.707f, 0.0f), glm::vec3(0.707f, 0.707f, 0.0f), glm::vec3(0.707f, 0.707f, 0.0f),

        // Back
        glm::vec3(0.0f, 0.707f, -0.707f), glm::vec3(0.0f, 0.707f, -0.707f), glm::vec3(0.0f, 0.707f, -0.707f),

        // Left
        glm::vec3(-0.707f, 0.707f, 0.0f), glm::vec3(-0.707f, 0.707f, 0.0f), glm::vec3(-0.707f, 0.707f, 0.0f),
    };

    uvs = {
        // Base
        glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
        glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f),

        // Front
        glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(0.5f, 1.0f),

        // Right
        glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(0.5f, 1.0f),

        // Back
        glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(0.5f, 1.0f),

        // Left
        glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(0.5f, 1.0f),
    };

    pyramid_elements = {
        // Base
        0, 1, 2, 0, 2, 3,

        // Front
        4, 5, 6,

        // Right
        7, 8, 9,

        // Back
        10, 11, 12,

        // Left
        13, 14, 15
    };
    texture_id = loadBMP(texture.c_str());
    initMesh();
}

void Pyramid::initMesh() {
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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, pyramid_elements.size() * sizeof(GLushort), pyramid_elements.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void Pyramid::render(glm::mat4 matrix, GLuint program_id, glm::mat4 view, glm::mat4 projection, LightSources lights) {
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
    glDrawElements(GL_TRIANGLES, pyramid_elements.size(), GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
}

Pyramid::Pyramid(const Pyramid& other) {
    this->vertices = other.vertices;
    this->normals = other.normals;
    this->uvs = other.uvs;
    this->pyramid_elements = other.pyramid_elements;
    this->material = other.material;
    this->usesTexture = other.usesTexture;

    if (this->usesTexture) {
        this->texture_id = other.texture_id;
    }

    this->initMesh();
}
Pyramid* Pyramid::clone() {
    return new Pyramid(*this);
}