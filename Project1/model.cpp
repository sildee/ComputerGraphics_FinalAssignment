#include "model.h"
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
#include "mesh.h"
#include "lightsource.h"

void Model::render(GLuint program_id, glm::mat4 view, glm::mat4 projection, LightSources lights) {
    for (auto& mesh : meshes) {
        mesh->render(matrix, program_id, view, projection, lights);
    }
}

Model::Model(const vector<Mesh*>& meshList, glm::vec3 position) {
	this->position = position;
    matrix = glm::translate(glm::mat4(1.0f), position);
    meshes = meshList;
}
void Model::animate() {
    matrix = glm::rotate(matrix, 0.01f, glm::vec3(0.0f, 1.0f, 0.0f));
}
void Model::scale(const glm::vec3& factor) {
	matrix = glm::scale(matrix, factor);
}
void Model::add(Mesh* mesh) {
	meshes.push_back(mesh);
}
void Model::rotate(const float radians, const glm::vec3& rotation) {
	matrix = glm::rotate(matrix, radians, rotation);
}
void Model::translate(const glm::vec3& translation) {
	matrix = glm::translate(matrix, translation);
	this->position += translation;
}
Model* Model::clone() {
	Model* newModel = new Model( {}, this->position);
	for (auto& mesh : this->meshes) {
		newModel->add(mesh->clone());
	}
	newModel->matrix = this->matrix;
	return newModel;
}
