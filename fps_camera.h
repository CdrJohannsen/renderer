#pragma once

#include "camera.h"
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

class FPSCamera : public Camera {
    public:

        FPSCamera(float fov, float width, float heigth) : Camera(fov,width,heigth) {
            up = glm::vec3(0.0f,1.0f,0.0f);
            yaw = -90.0f;
            pitch = 0.0f;
            onMouseMove(0.0f,0.0f);
            update();
        }

        void onMouseMove(float xRel, float yRel){
            yaw += xRel * mouseSensitivity;
            pitch -= yRel * mouseSensitivity;
            if (pitch > 89) {pitch = 89;}
            if (pitch < -89) {pitch = -89;}
            if (yaw < 0) {yaw += 360;}
            if (yaw > 360) {yaw -= 360;}
            glm::vec3 front;
            front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
            front.y = sin(glm::radians(pitch));
            front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
            lookAt = glm::normalize(front);
            update();
        }

        void update() override {
            view = glm::lookAt(position,position+lookAt,up);
            viewProj = projection*view;
        }

        void moveFront(float amount){
            translate(glm::normalize(glm::vec3(1.0f,0.0f,1.0f)*lookAt) * amount);
            update();
        }

        void moveSide(float amount){
            translate(glm::normalize(glm::cross(lookAt,up))*amount);
            update();
        }

        string getViewString(){
            string a = "yaw: ";
            a.append(to_string(yaw));
            a.append(" pitch: ");
            a.append(to_string(pitch));
            return a;
        }

        string getLookAtString(){
            string a = "x: ";
            a.append(to_string(lookAt.x));
            a.append(" y: ");
            a.append(to_string(lookAt.y));
            a.append(" z: ");
            a.append(to_string(lookAt.z));
            return a;
        }

    protected:
        float yaw;
        float pitch;
        glm::vec3 lookAt;
        const float mouseSensitivity = 0.3f;
        glm::vec3 up;
};
