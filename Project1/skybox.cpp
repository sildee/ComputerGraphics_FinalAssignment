#include "skybox.h"

//The skybox is partially based on https://www.mbsoftworks.sk/tutorials/opengl3/10-skybox/ and partially my own code / code from the lectures.
//To handle the (lack of) lighting I changed the fragmentshader with another if statement instead of making a second one.
//I also discussed the above resource / the skybox with Doede Dijkstra as we were both struggling with it
void Skybox::initMesh() {
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
Skybox::Skybox(const string front, const string back, const string left, const string right, const string top, const string bottom) {
    textureIDs[0] = texture_id = loadBMP(front.c_str());
    textureIDs[1] = texture_id = loadBMP(back.c_str());
    textureIDs[2] = texture_id = loadBMP(left.c_str());
    textureIDs[3] = texture_id = loadBMP(right.c_str());
    textureIDs[4] = texture_id = loadBMP(top.c_str());
    textureIDs[5] = texture_id = loadBMP(bottom.c_str());

    material.ambient_color = glm::vec3(1,1,1);
    material.diffuse_color = glm::vec3(0.6, 0.6, 0.6);
    material.specular = glm::vec3(1, 1, 1);
    material.power = 128;

    vertices =
    {
        // Front face
        glm::vec3(100.0f, 100.0f, 100.0f), glm::vec3(100.0f, -100.0f, 100.0f), glm::vec3(-100.0f, 100.0f, 100.0f), glm::vec3(-100.0f, -100.0f, 100.0f),
        // Back face
        glm::vec3(-100.0f, 100.0f, -100.0f), glm::vec3(-100.0f, -100.0f, -100.0f), glm::vec3(100.0f, 100.0f, -100.0f), glm::vec3(100.0f, -100.0f, -100.0f),
        // Left face
        glm::vec3(-100.0f, 100.0f, 100.0f), glm::vec3(-100.0f, -100.0f, 100.0f), glm::vec3(-100.0f, 100.0f, -100.0f), glm::vec3(-100.0f, -100.0f, -100.0f),
        // Right face
        glm::vec3(100.0f, 100.0f, -100.0f), glm::vec3(100.0f, -100.0f, -100.0f), glm::vec3(100.0f, 100.0f, 100.0f), glm::vec3(100.0f, -100.0f, 100.0f),
        // Top face
        glm::vec3(-100.0f, 100.0f, -100.0f), glm::vec3(100.0f, 100.0f, -100.0f), glm::vec3(-100.0f, 100.0f, 100.0f), glm::vec3(100.0f, 100.0f, 100.0f),
        // Bottom face
        glm::vec3(100.0f, -100.0f, -100.0f), glm::vec3(-100.0f, -100.0f, -100.0f), glm::vec3(100.0f, -100.0f, 100.0f), glm::vec3(-100.0f, -100.0f, 100.0f),
    };
    normals = {
        glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)
    };uvs = {
            // Front face
            glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f),
            // Back face
            glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f),
            // Left face
            glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f),
            // Right face
            glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f),
            // Top face
            glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
            // Bottom face
            glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f),
    };
    initMesh();
}
void Skybox::render(glm::mat4 matrix, GLuint program_id, glm::mat4 view, glm::mat4 projection, LightSources lights) {
    glUseProgram(program_id);

    GLuint viewLoc = glGetUniformLocation(program_id, "view");
    GLuint projLoc = glGetUniformLocation(program_id, "projection");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    GLuint uniform_material_ambient = glGetUniformLocation(program_id, "mat_ambient");
    GLuint uniform_material_diffuse = glGetUniformLocation(program_id, "mat_diffuse");
    GLuint uniform_specular = glGetUniformLocation(program_id, "mat_specular");
    GLuint uniform_material_power = glGetUniformLocation(program_id, "mat_power");
    GLuint uniform_uses_texture = glGetUniformLocation(program_id, "model_uses_texture");
    GLuint uniform_is_skybox = glGetUniformLocation(program_id, "model_is_skybox");

    glUniform3fv(uniform_material_ambient, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
    glUniform3fv(uniform_material_diffuse, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
    glUniform3fv(uniform_specular, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
    glUniform1f(uniform_material_power, 1.0f);
    glUniform1i(uniform_uses_texture, 1);
    glUniform1i(uniform_is_skybox, 1);

    glDepthMask(GL_FALSE);
    glBindVertexArray(vao);

    for (int i = 0; i < 6; ++i) {
        glBindTexture(GL_TEXTURE_2D, textureIDs[i]);
        glDrawArrays(GL_TRIANGLE_STRIP, i*4, 4);
    }

    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glUniform1i(uniform_is_skybox, 0);
    glUseProgram(0);
}