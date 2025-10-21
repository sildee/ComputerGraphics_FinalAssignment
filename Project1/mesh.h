#pragma once
#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glsl.h"
#include "objloader.h"
#include "texture.h"
#include "material.h"
#include "lightsource.h"

class Mesh {
private:

public:
	vector<glm::vec3> vertices, normals;
	vector<glm::vec2> uvs;
	GLuint vao, vbo, texture_id;
	Material material;
	int usesTexture;
	int isSkybox = 0;

	virtual void initMesh();
	Mesh(const string& mesh, const string& texture);
	Mesh(const string& mesh);
	Mesh(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& uvs, const string& texture);
	Mesh();
	void setDefaultMaterial();
	void scale(const glm::vec3& factor);
	void translate(const glm::vec3& translation);
	void rotate(const float radians, const glm::vec3& rotation);
	void handleLights(GLuint program_id, LightSources lights);
	virtual void render(glm::mat4 matrix, GLuint program_id, glm::mat4 view, glm::mat4 projection, LightSources lights);
	virtual Mesh* clone();
};