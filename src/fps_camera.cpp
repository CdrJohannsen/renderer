#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"
#include "fps_camera.hpp"

FPSCamera::FPSCamera(float fov, float width, float heigth) : Camera(fov, width, heigth) {
    up = glm::vec3(0.0f, 1.0f, 0.0f);
    yaw = -90.0f;
    pitch = 0.0f;
    onMouseMove(0.0f, 0.0f);
    update();
}

void FPSCamera::onMouseMove(float xRel, float yRel) {
    yaw += xRel * mouseSensitivity;
    pitch -= yRel * mouseSensitivity;
    if (pitch > 89) {
        pitch = 89;
    }
    if (pitch < -89) {
        pitch = -89;
    }
    if (yaw < 0) {
        yaw += 360;
    }
    if (yaw > 360) {
        yaw -= 360;
    }
    glm::vec3 front;
    front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    front.y = sin(glm::radians(pitch));
    front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    lookAt = glm::normalize(front);
    update();
}

void FPSCamera::update() {
    view = glm::lookAt(position, position + lookAt, up);
    viewProj = projection * view;
}

void FPSCamera::moveFront(float amount) {
    translate(glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f) * lookAt) * amount);
    update();
}

void FPSCamera::moveSide(float amount) {
    translate(glm::normalize(glm::cross(lookAt, up)) * amount);
    update();
}

string FPSCamera::getViewString() {
    string a = "yaw: ";
    a.append(to_string(yaw));
    a.append(" pitch: ");
    a.append(to_string(pitch));
    return a;
}

string FPSCamera::getLookAtString() {
    string a = "x: ";
    a.append(to_string(lookAt.x));
    a.append(" y: ");
    a.append(to_string(lookAt.y));
    a.append(" z: ");
    a.append(to_string(lookAt.z));
    return a;
}
