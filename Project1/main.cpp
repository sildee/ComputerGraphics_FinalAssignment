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
#include "mesh.h"
#include "lightsource.h"
#include "model.h"
#include "cube.h"
#include "skybox.h"
#include "cone.h"
#include "cylinder.h"
#include "pyramid.h"
#include "plane.h"
#include <string>
using namespace std;

//Textures obtained from freepik.com and polyhaven.com free license libraries
//Map based on https://www.reddit.com/r/wow/comments/kxtkhk/stormwind_city_by_dongming_yang/
//--------------------------------------------------------------------------------
// Consts
//--------------------------------------------------------------------------------

const int WIDTH = 1280, HEIGHT = 720;

const char* fragshader_name = "fragmentshader.frag";
const char* vertexshader_name = "vertexshader.vert";

unsigned const int DELTA_TIME = 10;


vector<Model> models;
LightSources lights;
glm::mat4 view, projection;
GLuint program_id;
//--------------------------------------------------------------------------------
// Keyboard handling and camera logic
// Camera logic based on https://learnopengl.com/Getting-Started/Camera
// Modified for own implementation (keyboard instead of camera and prevent moving on the y axis, handle y-axis movement in drone mode)
//--------------------------------------------------------------------------------
const float moveSpeed = 0.25f;
float yaw = 90.0f;
float pitch = 0.0f;
bool walkMode = true;
glm::vec3 direction;
glm::vec3 oldCameraPos, cameraPos, cameraFront, cameraUp;
float oldPitch, oldYaw;

void updateDirection() {
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

void keyboardHandler(unsigned char key, int a, int b)
{
    float y = cameraPos.y;
    updateDirection();
    if (key == 27)
        glutExit();
	if (key == 'w') {
        cameraPos += moveSpeed * cameraFront;
	}
    if (key == 's') {
        cameraPos -= moveSpeed * cameraFront;
    }
    if (key == 'a') {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * moveSpeed;
    }
    if (key == 'd') {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * moveSpeed;
    }
    if (key == 'l') {
        yaw += 1.5;
    }
    if (key == 'j') {
        yaw -= 1.5;
    }
    if (key == 'i') {
        pitch += 1.5;
    }
    if (key == 'k') {
        pitch -= 1.5;
    }
    if (key == 'v') {
        if (walkMode) {
            oldCameraPos = cameraPos;
            oldPitch = pitch;
            oldYaw = yaw;
            cameraPos = glm::vec3(0, 10, -13);
            cameraFront = glm::vec3(0, 0, 1);
            yaw = 90;
            pitch = -16;
        } else {
            cameraPos = oldCameraPos;
            yaw = oldYaw;
            pitch = oldPitch;
        }
        walkMode = !walkMode;
    }
    if (walkMode && key != 'v') {
        cameraPos.y = y;
    } else {
        if (key == 'q') {
            cameraPos.y += moveSpeed;
        }
        if (key == 'e') {
            cameraPos.y -= moveSpeed;
        }

    }
    updateDirection();
}

//--------------------------------------------------------------------------------
// Animations
//--------------------------------------------------------------------------------

const float animationSpeed = 0.01f;

// 'Secret' (Statue)
Model* easterStatue;
glm::vec3 statuePos;
float timeSinceLastTurn = 0.0f;
float turnCooldown = 10.0f;
bool isTurning = false;
float turnDuration = 1.5f;
float turnElapsed = 0.0f;
float startAngle = 0.0f;
float targetAngle = 0.0f;
float currentRotation = 0.0f;

// GenAI was used with this function to help me calculate the turn angles. glm::abs and glm::mix were new to me but key to make it work
// Purpose of the function:: every ~10 seconds, the easter statue turns in place to face the camera.
// It doesn't work perfectly, often rotating more than it should - but it's honestly quite funny so I wanted to keep it like this
void AnimateStatue() {
    timeSinceLastTurn += animationSpeed;

    if (timeSinceLastTurn >= turnCooldown && !isTurning) {
        glm::vec3 dir = glm::normalize(cameraPos - statuePos);
        targetAngle = atan2(dir.x, dir.z); // Yaw
        startAngle = currentRotation;
        turnElapsed = 0.0f;
        isTurning = true;
        timeSinceLastTurn = 0.0f;
    }

    if (isTurning) {
        if (std::abs(currentRotation - targetAngle) < animationSpeed) {
            currentRotation = targetAngle;
            isTurning = false;
        }
        else {
            currentRotation = glm::mix(startAngle, targetAngle, turnElapsed / turnDuration);
            turnElapsed += animationSpeed;
        }

        // Directly set the model's matrix with translation and rotation
        easterStatue->matrix = glm::translate(glm::mat4(1.0f), statuePos) * glm::rotate(glm::mat4(1.0f), currentRotation, glm::vec3(0.0f, 1.0f, 0.0f));
    }
}
//--------------------------------------------------------------------------------
// Bird
//--------------------------------------------------------------------------------

Model* animatedBird;
float birdAngle = 0.0f;
glm::vec3 birdStartingPosition = glm::vec3(0, 12, 15);
void AnimateBirds() {
    if (animatedBird) {
        birdAngle += animationSpeed;

        float radius = 10.0f;
        float x = radius * cos(birdAngle);
        float z = radius * sin(birdAngle);
        float y = 0.0f;

        glm::vec3 newPosition = birdStartingPosition - glm::vec3(x, y, z);

        animatedBird->matrix = glm::mat4(1.0f);
        animatedBird->translate(newPosition);

        float yaw = -birdAngle + glm::pi<float>() / 2;
        animatedBird->rotate(yaw, glm::vec3(0, 1, 0));
    }
}

int flickerCounter = 0;
const int flickerInterval = 15;
void FlickerLights(int i) {
    if (flickerCounter % flickerInterval == 0) {
        float intensity = lights[i].intensity;
        float flicker = (rand() % 20 - 10) / 100.0f;
        lights[i].intensity = glm::clamp(intensity + flicker, 0.15f, 0.30f);
    }

}

//--------------------------------------------------------------------------------
// Rendering
//--------------------------------------------------------------------------------

void Render() {
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, glm::vec3(0.0, 1.0, 0.0));

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    AnimateBirds();
    AnimateStatue();

    int num_lights = lights.size();
    flickerCounter++;
    for (size_t i = 0; i < num_lights; ++i) {
        FlickerLights(i);
        std::string uniform_name = "light_positions[" + std::to_string(i) + "]";
        GLuint uniform_location = glGetUniformLocation(program_id, uniform_name.c_str());
        glUniform3fv(uniform_location, 1, glm::value_ptr(glm::vec3(view * glm::vec4(lights[i].position, 1.0f))));

        std::string color_name = "light_colors[" + std::to_string(i) + "]";
        GLuint color_loc = glGetUniformLocation(program_id, color_name.c_str());
        glUniform3fv(color_loc, 1, glm::value_ptr(lights[i].color));

        std::string intensity_name = "light_intensities[" + std::to_string(i) + "]";
        GLuint intensity_loc = glGetUniformLocation(program_id, intensity_name.c_str());
        glUniform1f(intensity_loc, lights[i].intensity);
    }
    GLuint num_lights_location = glGetUniformLocation(program_id, "num_lights");
    glUniform1i(num_lights_location, num_lights);

    for (auto& model : models) {
        model.render(program_id, view, projection, lights);
    }
    glutSwapBuffers();
}
//------------------------------------------------------------
// void Render(int n)
// Render method that is called by the timer function
//------------------------------------------------------------

void Render(int n)
{
    Render();
    glutTimerFunc(DELTA_TIME, Render, 0);
}


//------------------------------------------------------------
// void InitGlutGlew(int argc, char **argv)
// Initializes Glut and Glew
//------------------------------------------------------------

void InitGlutGlew(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Hello OpenGL");

    glewInit();
    glutDisplayFunc(Render);
    glutKeyboardFunc(keyboardHandler);
    glutTimerFunc(DELTA_TIME, Render, 0);

    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);
}


//------------------------------------------------------------
// void InitShaders()
// Initializes the fragmentshader and vertexshader
//------------------------------------------------------------

void InitShaders()
{
    char* vertexshader = glsl::readFile(vertexshader_name);
    GLuint vsh_id = glsl::makeVertexShader(vertexshader);

    char* fragshader = glsl::readFile(fragshader_name);
    GLuint fsh_id = glsl::makeFragmentShader(fragshader);

    program_id = glsl::makeShaderProgram(vsh_id, fsh_id);
}


//------------------------------------------------------------
// void InitMatrices()
//------------------------------------------------------------

void InitMatrices()
{
    cameraPos = glm::vec3(0.0f, 1.75f, -7.0f);
    cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    updateDirection();
    view = glm::lookAt(cameraPos, cameraPos + cameraFront,cameraUp);
    projection = glm::perspective(
        glm::radians(45.0f),
        1.0f * WIDTH / HEIGHT, 0.1f,
        200.0f);
}

//------------------------------------------------------------
// Fill the world with models
//------------------------------------------------------------

void CreateSkyboxAndFloor() {
    Mesh* plane = new Plane("Textures/brickroad.bmp", 6, 30);
    plane->translate(glm::vec3(0, 0, 15));
    Mesh* forest = new Plane("Textures/outside.bmp", 60, 300);
    forest->translate(glm::vec3(0, -2, 0));
    Mesh* skybox = new Skybox("Textures/Skybox/front.bmp", "Textures/Skybox/back.bmp", "Textures/Skybox/left.bmp", "Textures/Skybox/right.bmp", "Textures/Skybox/top.bmp", "Textures/Skybox/bottom.bmp");
    Model base({ plane, forest, skybox }, glm::vec3(0, 0, 0));
    models.push_back(base);
}
void CreateRoad() {
    Mesh* bricks = new Cube("Textures/whitebricks.bmp", 0.4f, 0.2f, 30);
    bricks->translate(glm::vec3(3, 0.2f, 0));
    Mesh* bricks2 = new Cube("Textures/whitebricks.bmp", 1, 0.3f, 30);
    bricks2->translate(glm::vec3(5.8, 0.3f, 0));
    Mesh* tiles = new Plane("Textures/brownroad.bmp", 0.8f, 30);
    tiles->translate(glm::vec3(4, 0.399f, 0));


    Model roadModel({ bricks, bricks2, tiles }, glm::vec3(0, 0, 0));
    roadModel.translate(glm::vec3(0, 0, 15));
    Model* roadModel2 = roadModel.clone();
    roadModel2->rotate(glm::pi<float>(), glm::vec3(0, 1, 0));
    models.push_back(roadModel);
    models.push_back(*roadModel2);

}

void CreateLamps() {
    Mesh* lamppost = new Mesh("Objects/Lamppost3.obj", "Textures/lamp.bmp");
    lamppost->material.power = 64;
    lamppost->material.ambient_color = glm::vec3(0.1f, 0.1f, 0.1f);
    Mesh* candle = new Mesh("Objects/Candle.obj", "Textures/candle.bmp");
    lamppost->scale(glm::vec3(0.1f, 0.1f, 0.1f));
    candle->scale(glm::vec3(0.15f, 0.15f, 0.15f));
    glm::vec3 candleTranslation = glm::vec3(0.35f, 2.6f, -0.1f);
    candle->translate(candleTranslation);
    Model lamp({ lamppost, candle }, glm::vec3(-4.8, 0, -5));
    models.push_back(lamp);

    glm::vec3 candlePos = lamp.position + candleTranslation;
    glm::vec3 candleTranslationOpposite = glm::vec3(-0.35f, 2.6f, 0.1f);
	glm::vec3 candleColor = glm::vec3(0.6f, 0.4f, 0.4f);
	lights.push_back({ candlePos, candleColor, 0.3f });
    for (int i = 0; i < 4; ++i) {
        //Create lamps and light sources
        Model* newLamp = lamp.clone();
        newLamp->translate(glm::vec3(0, 0, 9 * i));
        models.push_back(*newLamp);
        candlePos = newLamp->position + candleTranslation;
        lights.push_back({ candlePos, candleColor, 0.3f });

        //Create lamps and light sources on the other side
        Model* oppositeLamp = lamp.clone();
        oppositeLamp->translate(glm::vec3(9.8, 0, 9 * i));
        oppositeLamp->rotate(glm::pi<float>(), glm::vec3(0, 1, 0));
        candlePos = oppositeLamp->position + candleTranslationOpposite;
        lights.push_back({ candlePos, glm::vec3(0.2f, 0.6f, 0.6f), 0.3f});
        models.push_back(*oppositeLamp);
    }
}

void CreateStatues() {
    //by Alok on Sketchfab https://sketchfab.com/3d-models/statue-a77d9a22ca964a71ab23519cab124ff6
	Mesh* garg = new Mesh("Objects/statue.obj", "Textures/whitebricks.bmp");
    garg->material.power = 128;
    garg->material.ambient_color = glm::vec3(0.3f, 0.3f, 0.4f);
	garg->rotate(glm::pi<float>()/2, glm::vec3(0, 1, 0));
    garg->scale(glm::vec3(2, 2, 2));
	Model gargModel({ garg }, glm::vec3(-6, 0.5, 7.75));
	models.push_back(gargModel);

    //by GrimmGitz on Sketchfab https://sketchfab.com/3d-models/easter-island-head-63b114c5e7d24f7fae421dfacfe41376
    Mesh* easter = new Mesh("Objects/easterislandhead.obj", "Textures/whitebricks.bmp");
    easter->scale(glm::vec3(0.05f, 0.05f, 0.05f));
    Model easterModel({ easter }, glm::vec3(6, 2, 7.75));
    easterModel.rotate((glm::pi<float>() / 2) * 3, glm::vec3(0, 1, 0));

    models.push_back(easterModel);
    easterStatue = &models.back();
    statuePos = easterStatue->position;
    currentRotation = (glm::pi<float>() / 2) * 3;


    //by ale.proglio on Sketchfab https://sketchfab.com/3d-models/statue-medford-square-in-alba-italy-b9923e6f00dc41f3aac9dff1cd95bf8c
    Mesh* bertero = new Mesh("Objects/bertero.obj", "Textures/whitebricks.bmp");
    bertero->material.power = 128;
    bertero->material.ambient_color = glm::vec3(0.2f, 0.3f, 0.3f);
    bertero->rotate(glm::pi<float>() / 6, glm::vec3(0, 1, 0));
    bertero->scale(glm::vec3(1.15, 1.15, 1.15));
    Model berteroModel({ bertero }, glm::vec3(5.5, 0.5, 17.75));
    models.push_back(berteroModel);

    //by Drone Magister on Sketchfab https://sketchfab.com/3d-models/rabelais-low-poly-d9576ee6f3df4ff3903865ddc227116b
    Mesh* rabelais = new Mesh("Objects/rabelais.obj", "Textures/whitebricks.bmp");
    rabelais->material.power = 256;
    rabelais->material.ambient_color = glm::vec3(0.05f, 0.1f, 0.05f);
    rabelais->rotate(glm::pi<float>() / 2, glm::vec3(0, 1, 0));
    rabelais->scale(glm::vec3(1.5, 1.5, 1.5));
    Model rabelaisModel({ rabelais }, glm::vec3(-6, 0.5, 27.75));
    models.push_back(rabelaisModel);
}
void CreateBirds() {
    Mesh* bird = new Mesh("Objects/gull.obj", "Textures/lamp.bmp");
	bird->scale(glm::vec3(5, 5, 5));
    bird->rotate(glm::pi<float>() / 2, glm::vec3(0, 1, 0));
    models.push_back(Model({bird}, birdStartingPosition));
    animatedBird = &models.back();
}

void CreateBackground() {
    Mesh* wall = new Mesh("Objects/wall.obj", "Textures/whitebricks.bmp");
    wall->scale(glm::vec3(0.2f, 0.2f, 0.3f));
    wall->rotate(glm::pi<float>() * 1.5, glm::vec3(0, 1, 0));
    Model wallModel({ wall }, glm::vec3(0, 0, 46));
    models.push_back(wallModel);

    Mesh* house = new Mesh("Objects/house.obj", "Textures/yellobrk.bmp");
    house->scale(glm::vec3(0.4f, 0.4f, 0.4f));
    house->rotate(glm::pi<float>() * 1.5, glm::vec3(0, 1, 0));

    Mesh* roof = new Mesh("Objects/roof.obj", "Textures/bluebrk.bmp");
    roof->material.diffuse_color = glm::vec3(0.2f, 0.2f, 0.7f);
    roof->scale(glm::vec3(0.4f, 0.4f, 0.4f));
    roof->rotate(glm::pi<float>() * 1.5, glm::vec3(0, 1, 0));

    Model houseModel({ house, roof }, glm::vec3(0, 0, 56));
    models.push_back(houseModel);
}
void CreateTowers() {
	Mesh* base = new Cylinder("Textures/whitebricks.bmp", 32, 1, 5);
	Mesh* top = new Cone("Textures/bluebrk.bmp", 32, 1, 1);
    top->translate(glm::vec3(0, 2.5, 0));

    Mesh* window = new Cube("textures/window.bmp");
    window->scale(glm::vec3(0.2f, 0.2f, 0.2f));
    window->translate(glm::vec3(0, 1.5f, -0.85f));

	Mesh* spike = new Pyramid("Textures/whitebricks.bmp");
    spike->rotate(glm::pi<float>() / 2, glm::vec3(1, 0, 0));
    spike->rotate(glm::pi<float>() / 2, glm::vec3(0, 1, 0));
    spike->scale(glm::vec3(0.5f, 0.5f, 0.5f));
    spike->translate(glm::vec3(1.0f, 1.5f, 0));

    Mesh* spike2 = new Pyramid("Textures/whitebricks.bmp");
    spike2->rotate(glm::pi<float>() * 1.5, glm::vec3(1, 0, 0));
    spike2->rotate(glm::pi<float>() / 2, glm::vec3(0, 1, 0));
    spike2->scale(glm::vec3(0.5f, 0.5f, 0.5f));
    spike2->translate(glm::vec3(-1.0f, 1.5f, 0));

	Model tower({  base, top, window, spike, spike2 }, glm::vec3(10, 5, 50));
    tower.scale(glm::vec3(2, 2, 2));

    Model* tower2 = tower.clone();
    tower2->translate(glm::vec3(-10, 0, 0));

    models.push_back(tower);
    models.push_back(*tower2);
}

void InitScene() {
    models.reserve(100);
    CreateSkyboxAndFloor();
    CreateRoad();
    CreateLamps();
    CreateBackground();
    CreateTowers();
    CreateBirds();
    CreateStatues();
    //moonlight

	lights.push_back({ glm::vec3(-200, 200, -200), glm::vec3(1, 1, 1), 0.1f });

}


int main(int argc, char** argv) {
    InitGlutGlew(argc, argv);
    InitMatrices();
    InitShaders();
    InitScene();
    glutMainLoop();
    return 0;
}
