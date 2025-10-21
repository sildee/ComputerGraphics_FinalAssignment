#include "cone.h"

//Cone vertices/normals made with assistance from GenAI as online resources for more complex 'primitives' were very limited
//and I honestly have no idea how to make normals and uvs myself, when I reached out to some other students they said they were having the same problem
Cone::Cone(const std::string& texture, int segments, float radius, float height) : Mesh() {
    setDefaultMaterial();
    texture_id = loadBMP(texture.c_str());
    usesTexture = 1;
    glm::vec3 top(0.0f, height, 0.0f);
    glm::vec3 base_center(0.0f, 0.0f, 0.0f);

    vertices.push_back(top);
    normals.push_back(glm::normalize(glm::vec3(0.0f, height * 0.5f, 0.0f)));
    uvs.push_back(glm::vec2(0.5f, 1.0f)); 

    float angleStep = 2 * glm::pi<float>() / segments;

    // Generate circle base vertices
    for (int i = 0; i <= segments; ++i) {
        float angle = i * angleStep;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        glm::vec3 position(x, 0.0f, z);
        vertices.push_back(position);

        // Normal (side normal approx halfway between up and outward)
        glm::vec3 side_dir = glm::normalize(glm::vec3(x, height, z));
        normals.push_back(side_dir);

        // UVs
        float u = (cos(angle) + 1.0f) * 0.5f;
        float v = (sin(angle) + 1.0f) * 0.5f;
        uvs.push_back(glm::vec2((float)i / segments, 0.0f));  // side UV
    }

    // Indices for side triangles (tip to base perimeter)
    for (int i = 1; i <= segments; ++i) {
        cone_elements.push_back(0);
        cone_elements.push_back(i);
        cone_elements.push_back(i + 1);
    }

    // Base center vertex
    GLuint base_center_index = vertices.size();
    vertices.push_back(base_center);
    normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
    uvs.push_back(glm::vec2(0.5f, 0.5f));

    // Base cap
    for (int i = 1; i <= segments; ++i) {
        cone_elements.push_back(base_center_index);
        cone_elements.push_back(i + 1);
        cone_elements.push_back(i);
    }
    initMesh();
}

void Cone::initMesh() {
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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cone_elements.size() * sizeof(GLushort), cone_elements.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void Cone::render(glm::mat4 matrix, GLuint program_id, glm::mat4 view, glm::mat4 projection, LightSources lights) {
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
    glDrawElements(GL_TRIANGLES, cone_elements.size(), GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
}
Cone::Cone(const Cone& other) {
    this->vertices = other.vertices;
    this->normals = other.normals;
    this->uvs = other.uvs;
    this->cone_elements = other.cone_elements;
    this->material = other.material;
    this->usesTexture = other.usesTexture;

    if (this->usesTexture) {
        this->texture_id = other.texture_id;
    }

    this->initMesh();
}
Cone* Cone::clone() {
    return new Cone(*this);
}