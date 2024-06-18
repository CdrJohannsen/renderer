#include "object.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "floating_camera.hpp"
#include "imgui.h"
#include "mesh.hpp"
#include "shader.hpp"

Object::Object(char *filename, Shader *shader, Shader *light_shader, glm::vec3 p, glm::vec3 r, glm::vec3 s) {
    model.init(filename, shader, light_shader);
    position = p;
    rotation = r;
    size = s;
    modelMat = glm::mat4(1.0f);
    // Scale here
    modelMat = glm::scale(modelMat, glm::vec3(1.0f));
    // Set uniforms
    modelViewLocation = glGetUniformLocation(shader->getShaderID(), "u_modelView");
    invModelViewLocation = glGetUniformLocation(shader->getShaderID(), "u_invModelView");
    modelViewProjLocation = glGetUniformLocation(shader->getShaderID(), "u_modelViewProj");
}

void Object::render(FloatingCamera camera) {
    modelViewProj = camera.getViewProj() * modelMat;
    modelView = camera.getView() * modelMat;
    invModelView = glm::transpose(glm::inverse(modelView));
    glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, &modelView[0][0]);
    glUniformMatrix4fv(invModelViewLocation, 1, GL_FALSE, &invModelView[0][0]);
    glUniformMatrix4fv(modelViewProjLocation, 1, GL_FALSE, &modelViewProj[0][0]);
    model.render();
}

void Object::updateLights(FloatingCamera camera) { model.updateLights(camera.getView(), modelMat); }

void Object::move(float x, float y, float z) {
    position += glm::vec3(x, y, z);
    modelMat = glm::translate(glm::mat4(1.0f), position);
}

void Object::scale(float x, float y, float z) {
    size += glm::vec3(x, y, z);
    modelMat = glm::scale(glm::mat4(1.0f), size);
}

void Object::rotate(float angle, float x, float y, float z) {
    rotation += glm::vec3(x, y, z);
    modelMat = glm::rotate(modelMat, angle, glm::vec3(x, y, z));
}

void Object::renderDebugUI() {
    if (ImGui::TreeNode("Name")) {
        ImGui::DragFloat("Pos X", &position.x, 0.5, -FLT_MAX, +FLT_MAX);
    }
}
