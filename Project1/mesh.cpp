#include "mesh.h"
#include <string>


void Mesh::setDefaultMaterial() {
    material.ambient_color = glm::vec3(0.225, 0.2, 0.2);
    material.diffuse_color = glm::vec3(0.6, 0.0, 0.0);
    material.specular = glm::vec3(1, 1, 1);
    material.power = 128;
}

void Mesh::initMesh() {
    GLuint vbo_vertices, vbo_normals, vbo_uvs;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vbo_normals);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &vbo_uvs);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_uvs);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
    
}
Mesh::Mesh(const string& mesh, const string& texture) {
    setDefaultMaterial();
	loadOBJ(mesh.c_str(), vertices, uvs, normals);
	texture_id = loadBMP(texture.c_str());
	usesTexture = 1;
	initMesh();
}
Mesh::Mesh(const string& mesh) {
    loadOBJ(mesh.c_str(), vertices, uvs, normals);
    usesTexture = 0;
    initMesh();
}
//for primitives
Mesh::Mesh(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& uvs, const string& texture) {
    this->vertices = vertices;
    this->normals = normals;
    this->uvs = uvs;
    this->texture_id = loadBMP(texture.c_str());
    usesTexture = 1;
    initMesh();
}
void Mesh::handleLights(GLuint program_id, LightSources lights) {
    for (size_t i = 0; i < lights.size(); ++i) {
        std::string index = std::to_string(i);
        GLuint uniform_light_pos = glGetUniformLocation(program_id, ("lights[" + index + "].position").c_str());
        GLuint uniform_light_color = glGetUniformLocation(program_id, ("lights[" + index + "].color").c_str());

        glUniform3fv(uniform_light_pos, 1, glm::value_ptr(lights[i].position));
        glUniform3fv(uniform_light_color, 1, glm::value_ptr(lights[i].color));
    }
}
void Mesh::render(glm::mat4 matrix, GLuint program_id, glm::mat4 view, glm::mat4 projection, LightSources lights) {

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

    if (usesTexture) glBindTexture(GL_TEXTURE_2D, texture_id);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    glBindVertexArray(0);
}

Mesh::Mesh() {
    vao = 0;
    vbo = 0;
    texture_id = 0;
    usesTexture = 0;
}

void Mesh::scale(const glm::vec3& factor) {
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), factor);

    for (int i = 0; i < vertices.size(); ++i) {
        glm::vec4 scaledVertex = scaleMatrix * glm::vec4(vertices[i], 1.0f);
        vertices[i] = glm::vec3(scaledVertex);
    }

	if (uvs.size() > 0) {
		for (int i = 0; i < uvs.size(); ++i) {
            uvs[i].x /= factor.x;
            uvs[i].y /= factor.y; 
		}
	}
    initMesh();
}

void Mesh::translate(const glm::vec3& translation) {
    glm::mat4 translateMatrix = glm::translate(glm::mat4(1.0f), translation);

    for (int i = 0; i < vertices.size(); ++i) {
        glm::vec4 translatedVertex = translateMatrix * glm::vec4(vertices[i], 1.0f);
        vertices[i] = glm::vec3(translatedVertex);
    }

    initMesh();
}

void Mesh::rotate(const float radians, const glm::vec3& rotation) {
	glm::mat4 rotateMatrix = glm::rotate(glm::mat4(1.0f), radians, rotation);
	for (int i = 0; i < vertices.size(); ++i) {
		glm::vec4 rotatedVertex = rotateMatrix * glm::vec4(vertices[i], 1.0f);
		vertices[i] = glm::vec3(rotatedVertex);
	}
	initMesh();
}
Mesh* Mesh::clone() {
	Mesh* newMesh = new Mesh();
	newMesh->vertices = this->vertices;
	newMesh->normals = this->normals;
	newMesh->uvs = this->uvs;
    newMesh->material = this->material;
	newMesh->texture_id = this->texture_id;
	newMesh->usesTexture = this->usesTexture;
	newMesh->initMesh();
	return newMesh;
}