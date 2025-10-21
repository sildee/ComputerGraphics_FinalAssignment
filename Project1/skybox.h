#include "mesh.h"
class Skybox : public Mesh {
private:
	std::vector<GLushort> cube_elements;
	GLuint textureIDs[6];
	int isSkybox = 1;

public:
	Skybox(const string front, const string back, const string left, const string right, const string top, const string bottom);
	void render(glm::mat4 matrix, GLuint program_id, glm::mat4 view, glm::mat4 projection, LightSources lights) override;
	void initMesh() override;
};