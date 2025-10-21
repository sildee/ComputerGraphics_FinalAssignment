#pragma once
#include "mesh.h"
#include "lightsource.h"
#include "cube.h"
class Model {
private:
public:
	glm::vec3 position;
	vector<Mesh*> meshes;
	glm::mat4 matrix;
	glm::mat4 rotationMatrix;
	Model(const vector<Mesh*>& meshList, glm::vec3 position);
	void render(GLuint program_id, glm::mat4 view, glm::mat4 projection, LightSources lights);
	void animate();
	void scale(const glm::vec3& factor);
	void translate(const glm::vec3& translation);
	void rotate(const float radians, const glm::vec3& rotation);
	void add(Mesh* mesh);
	Model* clone();
};