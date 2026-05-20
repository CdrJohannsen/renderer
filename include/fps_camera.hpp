#pragma once

#include <glm/glm.hpp>

#include "camera.hpp"

class FPSCamera : public Camera {
   public:
    FPSCamera(float fov, float width, float heigth);

    void onMouseMove(float xRel, float yRel);

    void update() override;

    void moveFront(float amount);

    void moveSide(float amount);

    string getViewString();

    string getLookAtString();

    const glm::vec3 &getLookAt() { return lookAt; }

   protected:
    float yaw;
    float pitch;
    glm::vec3 lookAt;
    const float mouseSensitivity = 0.3f;
    glm::vec3 up;
};
