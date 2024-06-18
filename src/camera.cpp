#include "camera.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

Camera::Camera(float fov, float width, float heigth) {
    projection = glm::perspective(fov / 2.0f, width / heigth, 0.2f, 500.0f);
    view = glm::mat4(1.0f);
    position = glm::vec3(0.0f, 0.0f, 5.0f);
    update();
}

glm::mat4 Camera::getViewProj() { return viewProj; }

glm::mat4 Camera::getProj() { return projection; }

glm::mat4 Camera::getView() { return view; }

void Camera::update() { viewProj = projection * view; }

void Camera::translate(glm::vec3 v) {
    position += v;
    view = glm::translate(view, v * -1.0f);
}
string Camera::getPositionString() {
    string a = "x: ";
    a.append(to_string(position.x));
    a.append(" y: ");
    a.append(to_string(position.y));
    a.append(" z: ");
    a.append(to_string(position.z));
    return a;
}
