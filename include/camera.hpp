#pragma once

#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
using namespace std;

class Camera {
   public:
    Camera(float fov, float width, float heigth);

    glm::mat4 getViewProj();

    glm::mat4 getProj();

    glm::mat4 getView();

    virtual void update();

    virtual void translate(glm::vec3 v);
    string getPositionString();

   protected:
    glm::vec3 position;
    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 viewProj;
};
