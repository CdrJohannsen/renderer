#pragma once
#include "floating_camera.hpp"
#include "mesh.hpp"
#include "shader.hpp"

class Object {
   public:
    Object(string n, char* filename, Shader* shader, Shader* light_shader, glm::vec3 p, glm::vec3 r = {0, 0, 0},
           glm::vec3 s = {1, 1, 1});

    void render(FloatingCamera camera);

    void updateLights(FloatingCamera camera);

    void move(float x, float y, float z);

    void scale(float x, float y, float z);

    void rotate(float angle, float x, float y, float z);

#ifdef _DEBUG
    void renderDebugUI();
#endif

   private:
    string name;
    Model model;
    glm::mat4 modelMat;
    glm::mat4 invModelView;
    glm::mat4 modelView;
    glm::mat4 modelViewProj;
    GLuint modelViewLocation;
    GLuint invModelViewLocation;
    GLuint modelViewProjLocation;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 size;
};
