#define GLEW_STATIC
#define AUDIO "resources/sounds/Joe Ford - Frozen Sound.mp3"

#include <iostream>
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SDL2/SDL_mixer.h>
#include "shader.h"
#include <vector>

struct action
{
    float starttime;
    float endtime;
    void(*ac_func) (float);
};
typedef struct action action;

struct camera
{
    float starttime;
    float endtime;
    glm::vec3 position;
    int interpolation;
};
typedef struct camera camera;

std::vector<action> action_list;
std::vector<camera> camera_list;

glm::vec3 camera_pos;
glm::vec3 prev_pos; // calcul de l'interpolation

GLFWwindow* window;

Mix_Music * _mmusic = NULL;
GLuint WIDTH = 800, HEIGHT = 600;

Shader lightingShader;
Shader lampShader;
Shader skyboxShader;
Shader cubemap;

GLuint sphereVBO, sphereVAO;
GLuint cubeVBO, cubeVAO;
GLuint skyboxVBO, skyboxVAO;
GLuint diffuseTEX, specuTEX;
GLuint skyboxTEX;

glm::mat4 model, view, projection;

std::vector<const GLchar*> faces;

GLfloat current_time;

int c_action;

GLfloat *ico_vert; // icosphere, va etre initialisé avec mksphere
GLfloat skyboxVertices[] = {
    // Positions          
    -1.0f, 1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,

    -1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, 1.0f
};

GLfloat vertices[] = {
    // Positions          // Normals           // Texture Coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};

glm::vec3 pointLightPositions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f),
    glm::vec3( 0.0f,  0.0f, 0.0f),
    glm::vec3( 0.0f,  0.0f, 0.0f),
    glm::vec3( 0.0f,  0.0f, 0.0f)
};

int init();
float map(float value, float min1, float max1, float min2, float max2);
void insert_action(float endtime, void(*ac_func) (float));
void play_action();
GLfloat * mkSphere(GLuint longitudes, GLuint latitudes);
GLuint loadTexture(GLchar const * path);
GLuint loadCubemap(std::vector<const GLchar*> faces);
void get_camera();
void insert_camera(float endtime, glm::vec3 position, int transision);
void keyback(GLFWwindow *windows, int key, int scancode, int action, int mods);
float randomfloat(float a, float b);
void resize(GLFWwindow *window, int width, int height);

void action1(float progression)
{
    cubemap.Use();
    float val = map(fmod(current_time, 5), 0, 5, 0, 2 * M_PI);

    model = glm::rotate(model, val, glm::vec3(1, 0, 0));
    model = glm::rotate(model, val, glm::vec3(1, 1, 0));
    model = glm::rotate(model, val, glm::vec3(1, 1, 1));

    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(glGetUniformLocation(cubemap.Program, "cameraPos"), camera_pos.x, camera_pos.y, camera_pos.z);
    // Cubes
    glBindVertexArray(cubeVAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTEX);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void action2(float progression)
{
    cubemap.Use();
    float val = map(fmod(current_time, 5), 0, 5, 0, 2 * M_PI);

    model = glm::rotate(model, val, glm::vec3(1, 0, 0));
    model = glm::rotate(model, val, glm::vec3(1, 1, 0));
    model = glm::rotate(model, val, glm::vec3(1, 1, 1));

    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(glGetUniformLocation(cubemap.Program, "cameraPos"), camera_pos.x, camera_pos.y, camera_pos.z);

    glBindVertexArray(cubeVAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTEX);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    lightingShader.Use();
    glUniform3f(glGetUniformLocation(lightingShader.Program, "viewPos"), camera_pos.x, camera_pos.y, camera_pos.z);

    float v = map(fmod(current_time, 3),0, 3, 0, 2 * M_PI);
    for(int i = 0; i < 3; i++)
    {
        pointLightPositions[i].x = 0;
        pointLightPositions[i].y = 0;
        pointLightPositions[i].z = 0;
    }
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);

    view = glm::lookAt(camera_pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTEX);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specuTEX);
    
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 1.73, 0));
    glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void action3(float progression)
{
    cubemap.Use();
    float val = map(fmod(current_time, 5), 0, 5, 0, 2 * M_PI);

    model = glm::rotate(model, val, glm::vec3(1, 0, 0));
    model = glm::rotate(model, val, glm::vec3(1, 1, 0));
    model = glm::rotate(model, val, glm::vec3(1, 1, 1));

    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(glGetUniformLocation(cubemap.Program, "cameraPos"), camera_pos.x, camera_pos.y, camera_pos.z);
    // Cubes
    glBindVertexArray(cubeVAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTEX);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);


    lightingShader.Use();
    glUniform1i(glGetUniformLocation(lightingShader.Program, "material.diffuse"),  0);
    glUniform1i(glGetUniformLocation(lightingShader.Program, "material.specular"), 1);
    glUniform3f(glGetUniformLocation(lightingShader.Program, "viewPos"), camera_pos.x, camera_pos.y, camera_pos.z);

    float v = map(fmod(current_time, 3),0, 3, 0, 2 * M_PI);
    for(int i = 0; i < 3; i++)
    {
        pointLightPositions[i].x = 0;
        pointLightPositions[i].y = 0;
        pointLightPositions[i].z = 0;
    }
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);

    view = glm::lookAt(camera_pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTEX);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specuTEX);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 1.73, 0));
    glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0, -1.73, 0));
    glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void action4(float progression)
{
    cubemap.Use();
    float val = map(fmod(current_time, 5), 0, 5, 0, 2 * M_PI);

    model = glm::rotate(model, val, glm::vec3(1, 0, 0));
    model = glm::rotate(model, val, glm::vec3(1, 1, 0));
    model = glm::rotate(model, val, glm::vec3(1, 1, 1));

    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(glGetUniformLocation(cubemap.Program, "cameraPos"), camera_pos.x, camera_pos.y, camera_pos.z);
    glBindVertexArray(cubeVAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTEX);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);


    lightingShader.Use();
    glUniform3f(glGetUniformLocation(lightingShader.Program, "viewPos"), camera_pos.x, camera_pos.y, camera_pos.z);

    float v = map(fmod(current_time, 3),0, 3, 0, 2 * M_PI);
    for(int i = 0; i < 3; i++)
    {
        pointLightPositions[i].x = 0;
        pointLightPositions[i].y = 0;
        pointLightPositions[i].z = 0;
    }
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);

    view = glm::lookAt(camera_pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTEX);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specuTEX);
    
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 1.73, 0));
    model = glm::rotate(model, val, glm::vec3(0, 1, 0));
    glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0, -1.73, 0));
    model = glm::rotate(model, -val, glm::vec3(0, 1, 0));
    glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void action5(float progression)
{
    cubemap.Use();
    float val = map(fmod(current_time, 5), 0, 5, 0, 2 * M_PI);

    model = glm::rotate(model, val, glm::vec3(1, 0, 0));
    model = glm::rotate(model, val, glm::vec3(1, 1, 0));
    model = glm::rotate(model, val, glm::vec3(1, 1, 1));

    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(glGetUniformLocation(cubemap.Program, "cameraPos"), camera_pos.x, camera_pos.y, camera_pos.z);
    glBindVertexArray(cubeVAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTEX);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);


    lightingShader.Use();
    glUniform3f(glGetUniformLocation(lightingShader.Program, "viewPos"), camera_pos.x, camera_pos.y, camera_pos.z);

    float v = map(fmod(current_time, 3),0, 3, 0, 2 * M_PI);
    for(int i = 0; i < 3; i++)
    {
        pointLightPositions[i].x = cos(v + map(i, 0, 3, 0, 2 * M_PI)) * progression * 5;
        pointLightPositions[i].y = 0;
        pointLightPositions[i].z = sin(v + map(i, 0, 3, 0, 2 * M_PI)) * progression * 5;
    }
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);

    view = glm::lookAt(camera_pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTEX);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specuTEX);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 1.73, 0));
    model = glm::rotate(model, val, glm::vec3(0, 1, 0));
    glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0, -1.73, 0));
    model = glm::rotate(model, -val, glm::vec3(0, 1, 0));
    glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);


    lampShader.Use();
    glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(sphereVAO);
    for(int i = 0; i < 3; i++)
    {
        glm::vec3 color = glm::vec3(0.2, 0.2, 0.2);
        color[i] = 0.8;
        glUniform3f(glGetUniformLocation(lampShader.Program, "color"), color.x, color.y, color.z);
        model = glm::mat4();
        model = glm::translate(model, pointLightPositions[i]);
        model = glm::scale(model, glm::vec3(0.2f));
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 6 * 3 * 16 * 16);
    }
    glBindVertexArray(0);
}

void action6(float progression)
{
    cubemap.Use();
    float val = map(fmod(current_time, 5), 0, 5, 0, 2 * M_PI);

    model = glm::rotate(model, val, glm::vec3(1, 0, 0));
    model = glm::rotate(model, val, glm::vec3(1, 1, 0));
    model = glm::rotate(model, val, glm::vec3(1, 1, 1));

    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(glGetUniformLocation(cubemap.Program, "cameraPos"), camera_pos.x, camera_pos.y, camera_pos.z);
    glBindVertexArray(cubeVAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTEX);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);


    lightingShader.Use();
    glUniform3f(glGetUniformLocation(lightingShader.Program, "viewPos"), camera_pos.x, camera_pos.y, camera_pos.z);

    float v = map(fmod(current_time, 3),0, 3, 0, 2 * M_PI);
    for(int i = 0; i < 3; i++)
    {
        pointLightPositions[i].x = cos(v + map(i, 0, 3, 0, 2 * M_PI)) * (1 - progression) * 5;
        pointLightPositions[i].y = 0;
        pointLightPositions[i].z = sin(v + map(i, 0, 3, 0, 2 * M_PI)) * (1 - progression) * 5;
    }
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);

    view = glm::lookAt(camera_pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTEX);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specuTEX);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 1.73, 0));
    model = glm::rotate(model, val, glm::vec3(0, 1, 0));
    glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0, -1.73, 0));
    model = glm::rotate(model, -val, glm::vec3(0, 1, 0));
    glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);


    lampShader.Use();
    glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glBindVertexArray(sphereVAO);
    for(int i = 0; i < 3; i++)
    {
        glm::vec3 color = glm::vec3(0.2, 0.2, 0.2);
        color[i] = 0.8;
        glUniform3f(glGetUniformLocation(lampShader.Program, "color"), color.x, color.y, color.z);
        model = glm::mat4();
        model = glm::translate(model, pointLightPositions[i]);
        model = glm::scale(model, glm::vec3(0.2f));
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 6 * 3 * 16 * 16);
    }
    glBindVertexArray(0);
}

void action7(float progression)
{
    cubemap.Use();
    float val = map(fmod(current_time, 5), 0, 5, 0, 2 * M_PI);

    model = glm::rotate(model, val, glm::vec3(1, 0, 0));
    model = glm::rotate(model, val, glm::vec3(1, 1, 0));
    model = glm::rotate(model, val, glm::vec3(1, 1, 1));

    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(glGetUniformLocation(cubemap.Program, "cameraPos"), camera_pos.x, camera_pos.y, camera_pos.z);

    glBindVertexArray(cubeVAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTEX);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);


    lightingShader.Use();
    glUniform3f(glGetUniformLocation(lightingShader.Program, "viewPos"), camera_pos.x, camera_pos.y, camera_pos.z);

    float v = map(fmod(current_time, 3),0, 3, 0, 2 * M_PI);
    for(int i = 0; i < 3; i++)
    {
        pointLightPositions[i].x = cos(v + map(i, 0, 3, 0, 2 * M_PI)) * (progression) * 5;
        pointLightPositions[i].y = progression * 5;
        pointLightPositions[i].z = sin(v + map(i, 0, 3, 0, 2 * M_PI)) * (progression) * 5;
    }
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);

    view = glm::lookAt(camera_pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTEX);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specuTEX);

    for(int i = 0; i < 10; i++)
    {
        model = glm::translate(glm::mat4(1.0f),
            glm::vec3(
                cos(map(i, 0, 10, 0, 2 * M_PI)) * map(progression, 0, 1, 0.2, 1) * 5,
                0,
                sin(map(i, 0, 10, 0, 2 * M_PI)) * map(progression, 0, 1, 0.2, 1) * 5));
        model = glm::rotate(model, -(float)glm::radians((float)i * (360 / 10)), glm::vec3(0, 1, 0));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindVertexArray(0);


    lampShader.Use();
    glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glBindVertexArray(sphereVAO);
    for(int i = 0; i < 3; i++)
    {
        glm::vec3 color = glm::vec3(0.2, 0.2, 0.2);
        color[i] = 0.8;
        glUniform3f(glGetUniformLocation(lampShader.Program, "color"), color.x, color.y, color.z);
        model = glm::mat4();
        model = glm::translate(model, pointLightPositions[i]);
        model = glm::scale(model, glm::vec3(0.2f));
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 6 * 3 * 16 * 16);
    }
    glBindVertexArray(0);
}

void action8(float progression)
{
    cubemap.Use();
    float val = map(fmod(current_time, 5), 0, 5, 0, 2 * M_PI);

    model = glm::rotate(model, val, glm::vec3(1, 0, 0));
    model = glm::rotate(model, val, glm::vec3(1, 1, 0));
    model = glm::rotate(model, val, glm::vec3(1, 1, 1));

    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(glGetUniformLocation(cubemap.Program, "cameraPos"), camera_pos.x, camera_pos.y, camera_pos.z);

    glBindVertexArray(cubeVAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTEX);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);


    lightingShader.Use();
    glUniform3f(glGetUniformLocation(lightingShader.Program, "viewPos"), camera_pos.x, camera_pos.y, camera_pos.z);

    float v = map(fmod(current_time, 3),0, 3, 0, 2 * M_PI);
    for(int i = 0; i < 3; i++)
    {
        pointLightPositions[i].x = cos(v + map(i, 0, 3, 0, 2 * M_PI)) * (1 - progression) * 5;
        pointLightPositions[i].y = progression * 5;
        pointLightPositions[i].z = sin(v + map(i, 0, 3, 0, 2 * M_PI)) * (1 - progression) * 5;
    }
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);

    view = glm::lookAt(camera_pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTEX);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specuTEX);

    for(int i = 0; i < 10; i++)
    {
        model = glm::translate(glm::mat4(1.0f),
            glm::vec3(
                cos(map(i, 0, 10, 0, 2 * M_PI)) * 5,
                0,
                sin(map(i, 0, 10, 0, 2 * M_PI)) * 5));
        model = glm::rotate(model, -(float)glm::radians((float)i * (360 / 10)), glm::vec3(0, 1, 0));
        model = glm::rotate(model, val, glm::vec3(0, 0, 1));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindVertexArray(0);


    lampShader.Use();
    glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    
    glBindVertexArray(sphereVAO);
    for(int i = 0; i < 3; i++)
    {
        glm::vec3 color = glm::vec3(0.2, 0.2, 0.2);
        color[i] = 0.8;
        glUniform3f(glGetUniformLocation(lampShader.Program, "color"), color.x, color.y, color.z);
        model = glm::mat4();
        model = glm::translate(model, pointLightPositions[i]);
        model = glm::scale(model, glm::vec3(0.2f));
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 6 * 3 * 16 * 16);
    }
    glBindVertexArray(0);
}

void action9(float progression)
{
    cubemap.Use();
    float val = map(fmod(current_time, 5), 0, 5, 0, 2 * M_PI);

    model = glm::rotate(model, val, glm::vec3(1, 0, 0));
    model = glm::rotate(model, val, glm::vec3(1, 1, 0));
    model = glm::rotate(model, val, glm::vec3(1, 1, 1));

    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(glGetUniformLocation(cubemap.Program, "cameraPos"), camera_pos.x, camera_pos.y, camera_pos.z);

    glBindVertexArray(cubeVAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTEX);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);


    lightingShader.Use();
    glUniform3f(glGetUniformLocation(lightingShader.Program, "viewPos"), camera_pos.x, camera_pos.y, camera_pos.z);

    float v = map(fmod(current_time, 3),0, 3, 0, 2 * M_PI);
    for(int i = 0; i < 3; i++)
    {
        pointLightPositions[i].x = randomfloat(-3.0f, 3.0f);
        pointLightPositions[i].y = randomfloat(-3.0f, 3.0f);
        pointLightPositions[i].z = randomfloat(-3.0f, 3.0f);
    }

    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);

    view = glm::lookAt(camera_pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTEX);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specuTEX);

    for(int i = 0; i < 10; i++)
    {
        model = glm::translate(glm::mat4(1.0f),
            glm::vec3(
                cos(map(i, 0, 10, 0, 2 * M_PI)) * 5,
                0,
                sin(map(i, 0, 10, 0, 2 * M_PI)) * 5));
        model = glm::rotate(model, -(float)glm::radians((float)i * (360 / 10)), glm::vec3(0, 1, 0));
        model = glm::rotate(model, val, glm::vec3(0, 0, 1));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindVertexArray(0);


    lampShader.Use();
    glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(sphereVAO);
    for(int i = 0; i < 3; i++)
    {
        glm::vec3 color = glm::vec3(0.2, 0.2, 0.2);
        color[i] = 0.8;
        glUniform3f(glGetUniformLocation(lampShader.Program, "color"), color.x, color.y, color.z);
        model = glm::mat4();
        model = glm::translate(model, pointLightPositions[i]);
        model = glm::scale(model, glm::vec3(0.2f));
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 6 * 3 * 16 * 16);
    }
    glBindVertexArray(0);
}

void action10(float progression)
{
    cubemap.Use();
    float val = map(fmod(current_time, 5), 0, 5, 0, 2 * M_PI);

    model = glm::rotate(model, val, glm::vec3(1, 0, 0));
    model = glm::rotate(model, val, glm::vec3(1, 1, 0));
    model = glm::rotate(model, val, glm::vec3(1, 1, 1));

    view = glm::rotate(view, map(progression, 0, 1, 0, 2*M_PI), glm::vec3(0, 0, 1));
    c_action = 1;

    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(glGetUniformLocation(cubemap.Program, "cameraPos"), camera_pos.x, camera_pos.y, camera_pos.z);

    glBindVertexArray(cubeVAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTEX);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);


    lightingShader.Use();
    glUniform3f(glGetUniformLocation(lightingShader.Program, "viewPos"), camera_pos.x, camera_pos.y, camera_pos.z);

    float v = map(fmod(current_time, 3),0, 3, 0, 2 * M_PI);
    for(int i = 0; i < 3; i++)
    {
        pointLightPositions[i].x = 0;
        pointLightPositions[i].y = cos(v + map(i, 0, 3, 0, 2 * M_PI)) * 5;
        pointLightPositions[i].z = sin(v + map(i, 0, 3, 0, 2 * M_PI)) * 5;
    }
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);

    glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTEX);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specuTEX);

    for(int i = 0; i < 10; i++)
    {
        model = glm::translate(glm::mat4(1.0f),
            glm::vec3(
                cos(map(i, 0, 10, 0, 2 * M_PI)) * 5,
                0,
                sin(map(i, 0, 10, 0, 2 * M_PI)) * 5));
        model = glm::rotate(model, -(float)glm::radians((float)i * (360 / 10)), glm::vec3(0, 1, 0));
        model = glm::rotate(model, val, glm::vec3(0, 0, 1));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindVertexArray(0);


    lampShader.Use();
    glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(sphereVAO);
    for(int i = 0; i < 3; i++)
    {
        glm::vec3 color = glm::vec3(0.2, 0.2, 0.2);
        color[i] = 0.8;
        glUniform3f(glGetUniformLocation(lampShader.Program, "color"), color.x, color.y, color.z);
        model = glm::mat4();
        model = glm::translate(model, pointLightPositions[i]);
        model = glm::scale(model, glm::vec3(0.2f));
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 6 * 3 * 16 * 16);
    }
    glBindVertexArray(0);

    glDepthFunc(GL_LEQUAL);
    skyboxShader.Use();
    view = glm::mat4(glm::mat3(glm::lookAt(camera_pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0))));
    view = glm::rotate(view, map(progression, 0, 1, 0, 2*M_PI), glm::vec3(0, 0, 1));
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTEX);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
    glfwSwapBuffers(window);
}

void action11(float progression)
{
    cubemap.Use();
    float val = map(fmod(current_time, 5), 0, 5, 0, 2 * M_PI);

    model = glm::rotate(model, val, glm::vec3(1, 0, 0));
    model = glm::rotate(model, val, glm::vec3(1, 1, 0));
    model = glm::rotate(model, val, glm::vec3(1, 1, 1));

    view = glm::rotate(view, map(progression, 0, 1, 0, 2*M_PI), glm::vec3(1, 0, 0));
    c_action = 1;

    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(glGetUniformLocation(cubemap.Program, "cameraPos"), camera_pos.x, camera_pos.y, camera_pos.z);

    glBindVertexArray(cubeVAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTEX);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthFunc(GL_LEQUAL);
    skyboxShader.Use();
    view = glm::mat4(glm::mat3(glm::lookAt(camera_pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0))));
    view = glm::rotate(view, map(progression, 0, 1, 0, 2*M_PI), glm::vec3(1, 0, 0));
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTEX);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
    glfwSwapBuffers(window);
}

void action12(float progression)
{
    cubemap.Use();
    float val = map(fmod(current_time, 5), 0, 5, 0, 2 * M_PI);

    model = glm::rotate(model, val, glm::vec3(1, 0, 0));
    model = glm::rotate(model, val, glm::vec3(1, 1, 0));
    model = glm::rotate(model, val, glm::vec3(1, 1, 1));


    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(glGetUniformLocation(cubemap.Program, "cameraPos"), camera_pos.x, camera_pos.y, camera_pos.z);

    glBindVertexArray(cubeVAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTEX);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);


    lightingShader.Use();
    glUniform3f(glGetUniformLocation(lightingShader.Program, "viewPos"), camera_pos.x, camera_pos.y, camera_pos.z);

    float v = map(fmod(current_time, 3),0, 3, 0, 2 * M_PI);
    for(int i = 0; i < 3; i++)
    {
        pointLightPositions[i].x = 0;
        pointLightPositions[i].y = cos(v + map(i, 0, 3, 0, 2 * M_PI)) * 5;
        pointLightPositions[i].z = sin(v + map(i, 0, 3, 0, 2 * M_PI)) * 5;
    }
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);

    glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTEX);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specuTEX);

    for(int i = 0; i < 10; i++)
    {
        model = glm::translate(glm::mat4(1.0f),
            glm::vec3(
                cos(val + map(i, 0, 10, 0, 2 * M_PI)) * 5,
                0,
                sin(val + map(i, 0, 10, 0, 2 * M_PI)) * 5));
        model = glm::rotate(model, -(float)glm::radians((float)i * (360 / 10)), glm::vec3(0, 1, 0));
        model = glm::rotate(model, val, glm::vec3(0, 0, 1));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindVertexArray(0);


    lampShader.Use();
    glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(sphereVAO);
    for(int i = 0; i < 3; i++)
    {
        glm::vec3 color = glm::vec3(0.2, 0.2, 0.2);
        color[i] = 0.8;
        glUniform3f(glGetUniformLocation(lampShader.Program, "color"), color.x, color.y, color.z);
        model = glm::mat4();
        model = glm::translate(model, pointLightPositions[i]);
        model = glm::scale(model, glm::vec3(0.2f));
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 6 * 3 * 16 * 16);
    }
    glBindVertexArray(0);
}

void action13(float progression)
{
    cubemap.Use();
    float val = map(fmod(current_time, 5), 0, 5, 0, 2 * M_PI);

    model = glm::rotate(model, val, glm::vec3(1, 0, 0));
    model = glm::rotate(model, val, glm::vec3(1, 1, 0));
    model = glm::rotate(model, val, glm::vec3(1, 1, 1));


    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(cubemap.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(glGetUniformLocation(cubemap.Program, "cameraPos"), camera_pos.x, camera_pos.y, camera_pos.z);

    glBindVertexArray(cubeVAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTEX);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);


    lightingShader.Use();
    glUniform3f(glGetUniformLocation(lightingShader.Program, "viewPos"), camera_pos.x, camera_pos.y, camera_pos.z);

    float v = map(fmod(current_time, 3),0, 3, 0, 2 * M_PI);
    for(int i = 0; i < 3; i++)
    {
        pointLightPositions[i].x = sin(v + map(i, 0, 3, 0, 2 * M_PI)) * 5;
        pointLightPositions[i].y = progression * -3.0f;
        pointLightPositions[i].z = cos(v + map(i, 0, 3, 0, 2 * M_PI)) * 5;
    }
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);

    glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseTEX);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specuTEX);

    for(int i = 0; i < 10; i++)
    {
        model = glm::translate(glm::mat4(1.0f),
            glm::vec3(
                cos(val + map(i, 0, 10, 0, 2 * M_PI)) * 5,
                sin(val + map(i, 0, 10, 0, 2 * M_PI)) * 5,
                0));
        model = glm::rotate(model, -(float)glm::radians((float)i * (360 / 10)), glm::vec3(0, 1, 0));
        model = glm::rotate(model, val, glm::vec3(0, 0, 1));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindVertexArray(0);


    lampShader.Use();
    glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(sphereVAO);
    for(int i = 0; i < 3; i++)
    {
        glm::vec3 color = glm::vec3(0.2, 0.2, 0.2);
        color[i] = 0.8;
        glUniform3f(glGetUniformLocation(lampShader.Program, "color"), color.x, color.y, color.z);
        model = glm::mat4();
        model = glm::translate(model, pointLightPositions[i]);
        model = glm::scale(model, glm::vec3(progression * 15.0f));
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 6 * 3 * 16 * 16);
    }
    glBindVertexArray(0);
}



int main()
{
    if(init() < 0)
        return -1;

    camera_pos = glm::vec3(0.0f, 0.0f, 0.0f);
    prev_pos = camera_pos;
    
    insert_camera(0.975f * 0.0f, glm::vec3(0.0f, 0.0f, 5.0f), 0);
    insert_camera(0.975f * 8.0f, glm::vec3(0.0f, 10.0f, 10.0f), 1);
    insert_camera(0.975f * 12.0f, glm::vec3(0.0f, 0.0f, 5.0f), 1);
    insert_camera(0.975f * 20.0f, glm::vec3(0.0f, 0.0f, 5.0f), 0);
    insert_camera(0.975f * 24.0f, glm::vec3(7.0f, 0.0f, 7.0f), 1);
    insert_camera(0.975f * 32.0f, glm::vec3(0.0f, 8.0f, 2.0f), 1);
    insert_camera(0.975f * 40.0f, glm::vec3(0.0f, 0.0f, 8.0f), 0);
    insert_camera(0.975f * 48.0f, glm::vec3(-5.0f, -2.0f, -3.0f), 1);
    insert_camera(0.975f * 52.0f, glm::vec3(0.0f, 0.0f, 20.0f), 1);
    insert_camera(0.975f * 58.0f, glm::vec3(0.0f, 10.0f, 20.0f), 1);

    insert_action(0.975f * 4.0f, action1);
    insert_action(0.975f * 8.0f, action2);
    insert_action(0.975f * 12.0f, action3);
    insert_action(0.975f * 16.0f, action4);
    insert_action(0.975f * 20.0f, action5);
    insert_action(0.975f * 24.0f, action6);
    insert_action(0.975f * 28.0f, action7);
    insert_action(0.975f * 32.0f, action8);
    insert_action(0.975f * 36.0f, action9);
    insert_action(0.975f * 40.0f, action10);
    insert_action(0.975f * 44.0f, action11);
    insert_action(0.975f * 48.0f, action12);
    insert_action(0.975f * 58.0f, action13);

    glfwSetTime(0);

    int frames = 0;
    float last_time = 0;

    while (!glfwWindowShouldClose(window))
    {
        frames++;
        current_time = glfwGetTime();
        if(current_time - last_time > 1)
        {
            last_time = current_time;
            
            printf("FPS : %d\n", frames);
            frames = 0;
        }
        glfwPollEvents();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        get_camera();
        model = glm::mat4();
        //camera_pos = glm::vec3(0, 7, 7);
        view = glm::lookAt(camera_pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));//camera.GetViewMatrix();
        
        projection = glm::perspective(45.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        c_action = 0;
        play_action();

        if(c_action == 0)
        {
            glDepthFunc(GL_LEQUAL);
            skyboxShader.Use();
            view = glm::mat4(glm::mat3(glm::lookAt(camera_pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0))));
            glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            // skybox cube
            glBindVertexArray(skyboxVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTEX);
            glDrawArrays(GL_TRIANGLES, 0, 36);
    
            glBindVertexArray(0);
            glDepthFunc(GL_LESS);
    
            glfwSwapBuffers(window);
        }
    }
    if(_mmusic) {
       if(Mix_PlayingMusic())
         Mix_HaltMusic();
       Mix_FreeMusic(_mmusic);
       _mmusic = NULL;
     }
    Mix_CloseAudio();
    Mix_Quit();

    glDeleteBuffers(1, &sphereVBO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &skyboxVBO);

    glDeleteProgram(lightingShader.Program);
    glDeleteProgram(lampShader.Program);
    glDeleteProgram(skyboxShader.Program);
    glDeleteProgram(cubemap.Program);

    glDeleteTextures(1, &diffuseTEX);
    glDeleteTextures(1, &specuTEX);
    glDeleteTextures(1, &skyboxTEX);

    glDeleteVertexArrays(1, &sphereVAO);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &skyboxVAO);

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

int init()
{
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    window = glfwCreateWindow(WIDTH, HEIGHT, "PG2D1516S2", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    glewInit();
    glfwSetKeyCallback(window, keyback);
    glfwSetWindowSizeCallback(window, resize);

    // Define the viewport dimensions
    glViewport(0, 0, WIDTH, HEIGHT);

    // OpenGL options
    glEnable(GL_TEXTURE_CUBE_MAP);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glfwSwapInterval(1); // activer la synchronisation verticale (même FPS que le rafraichissement de l'ecran)

    ico_vert = mkSphere(16, 16);

    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, 6 * 3 * 16 * 16 * sizeof *ico_vert, ico_vert, GL_STATIC_DRAW);
    glBindVertexArray(sphereVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glBindVertexArray(0);
    
    // Load textures
    faces.push_back("resources/skybox/right.jpg");
    faces.push_back("resources/skybox/left.jpg");
    faces.push_back("resources/skybox/top.jpg");
    faces.push_back("resources/skybox/bottom.jpg");
    faces.push_back("resources/skybox/back.jpg");
    faces.push_back("resources/skybox/front.jpg");
    glGenTextures(1, &diffuseTEX);
    glGenTextures(1, &specuTEX);
    diffuseTEX = loadTexture("resources/textures/container2.png");
    specuTEX = loadTexture("resources/textures/container2_specular.png");
    skyboxTEX = loadCubemap(faces);

    // shader loading
    lightingShader = Shader("shaders/lighting_maps.vs", "shaders/lighting_maps.frag");
    lampShader = Shader("shaders/lamp.vs", "shaders/lamp.frag");
    skyboxShader = Shader("shaders/skybox.vs", "shaders/skybox.frag");
    cubemap = Shader("shaders/cubemaps.vs", "shaders/cubemaps.frag");

    // Set texture units
    lightingShader.Use();
    glUniform1i(glGetUniformLocation(lightingShader.Program, "material.diffuse"),  0);
    glUniform1i(glGetUniformLocation(lightingShader.Program, "material.specular"), 1);
    glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);

    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), 0.8f, 0.0f, 0.0f);
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 1.0f, 0.5f, 0.5f);
    glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.09);
    glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.032);

    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), 0.0f, 0.8f, 0.0f);
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), 0.5f, 1.0f, 0.5f);
    glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.09);
    glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.032);

    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"), 0.05f, 0.05f, 0.05f);
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), 0.0f, 0.0f, 0.8f);
    glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), 0.5f, 0.5f, 1.0f);
    glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 0.09);
    glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 0.032);
    
    int mixFlags = MIX_INIT_OGG, res;
    res = Mix_Init(mixFlags);
    if( (res & mixFlags) != mixFlags ) {
      fprintf(stderr, "Mix_Init: Erreur lors de l'initialisation de la bibliotheque SDL_Mixer\n");
      fprintf(stderr, "Mix_Init: %s\n", Mix_GetError());
      exit(-3);
    }
    if(Mix_OpenAudio(22050, AUDIO_S16LSB, 2, 1024) < 0)
      exit(-4);
    if(!(_mmusic = Mix_LoadMUS(AUDIO))) {
      fprintf(stderr, "Erreur lors du Mix_LoadMUS: %s\n", Mix_GetError());
      exit(-5);
    }
    if(!Mix_PlayingMusic())
      Mix_PlayMusic(_mmusic, 1);
    
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }

    return 0;
}

float map(float value, float min1, float max1, float min2, float max2)
{
  return min2+(max2-min2)*((value-min1)/(max1-min1));
}

void get_camera()
{
    if(camera_list.empty())
        return;

    camera ccam = camera_list.front();
    if(current_time >= ccam.endtime)
    {
        prev_pos = ccam.position;
        camera_list.erase(camera_list.begin());
        return get_camera();
    }
    if(ccam.interpolation)
    {
        camera_pos = glm::vec3(
                map(current_time, ccam.starttime, ccam.endtime, prev_pos.x, ccam.position.x),
                map(current_time, ccam.starttime, ccam.endtime, prev_pos.y, ccam.position.y),
                map(current_time, ccam.starttime, ccam.endtime, prev_pos.z, ccam.position.z)
            );
    }
    else
        camera_pos = prev_pos;
}
void insert_camera(float endtime, glm::vec3 position, int transision)
{
    float starttime = 0.0f;
    if(!camera_list.empty())
        starttime = camera_list.back().endtime;
    else
        camera_pos = position;
    camera_list.push_back({starttime, endtime, position, transision});
}

void insert_action(float endtime, void(*ac_func) (float))
{
    float starttime = 0.0f;
    if(!action_list.empty())
        starttime = action_list.back().endtime;
    action_list.push_back({starttime, endtime, ac_func});
}

void play_action()
{
    if(action_list.empty())
    {
        glfwSetWindowShouldClose(window, 1);
        return;
    }
    action cAc = action_list.front();
    if(current_time >= cAc.endtime)
    {
        action_list.erase(action_list.begin());
        return play_action();
    }
    cAc.ac_func(map(current_time, cAc.starttime, cAc.endtime, 0, 1));
}

GLfloat * mkSphere(GLuint longitudes, GLuint latitudes) {
  int i, j, k;
  GLfloat phi, theta, r[2], x[2], y[2], z[2], * data;
  GLfloat c2MPI_Long = 2.0 * M_PI / longitudes;
  GLfloat cMPI_Lat = M_PI / latitudes;
  data = (GLfloat *)malloc(6 * 3 * longitudes * latitudes * sizeof *data);
  assert(data);
  for(i = 0, k = 0; i < latitudes; i++) {
    theta  = -M_PI_2 + i * cMPI_Lat;
    y[0] = sin(theta);
    y[1] = sin(theta + cMPI_Lat);
    r[0] = cos(theta);
    r[1] = cos(theta + cMPI_Lat);
    for(j = 0; j < longitudes; j++) {
      phi = j * c2MPI_Long;
      x[0] = cos(phi);
      x[1] = cos(phi + c2MPI_Long);
      z[0] = sin(phi);
      z[1] = sin(phi + c2MPI_Long);

      data[k++] = r[0] * x[0]; data[k++] = y[0];  data[k++] = r[0] * z[0];
      data[k++] = r[1] * x[1]; data[k++] = y[1];  data[k++] = r[1] * z[1];
      data[k++] = r[0] * x[1]; data[k++] = y[0];  data[k++] = r[0] * z[1];

      data[k++] = r[0] * x[0]; data[k++] = y[0];  data[k++] = r[0] * z[0];
      data[k++] = r[1] * x[0]; data[k++] = y[1];  data[k++] = r[1] * z[0];
      data[k++] = r[1] * x[1]; data[k++] = y[1];  data[k++] = r[1] * z[1];
    }
  }
  return data;
}

GLuint loadTexture(GLchar const * path)
{
    //Generate texture ID and load texture data 
    GLuint textureID;
    glGenTextures(1, &textureID);
    int width, height;
    unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(image);
    return textureID;
}

GLuint loadCubemap(std::vector<const GLchar*> faces)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE0);

    int width, height, a;
    unsigned char* image;

    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    for (GLuint i = 0; i < faces.size(); i++)
    {
        image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return textureID;
}

void keyback(GLFWwindow *windows, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(window, 1);
    if(action)
        printf("key : %d. scancode : %d. TIME : %f\n", key, scancode, glfwGetTime());
}

float randomfloat(float a, float b)
{
    return a + (rand() / (RAND_MAX / (b-a) ) ) ;  
}

void resize(GLFWwindow *window, int width, int height)
{
    WIDTH = width;
    HEIGHT = height;
    glViewport(0, 0, WIDTH, HEIGHT);
}