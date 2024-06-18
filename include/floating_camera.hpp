#pragma once
#include "fps_camera.hpp"

class FloatingCamera : public FPSCamera {
   public:
    FloatingCamera(float fov, float width, float heigth) : FPSCamera(fov, width, heigth) {}
    void moveUp(float amount) { translate(up * amount); }
};
