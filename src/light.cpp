#include "light.hpp"

#include <glm/glm.hpp>

#include "shader.hpp"

#ifdef _DEBUG
#include "imgui.h"
#endif

Light::Light(string name, Shader* shader, glm::vec3 c) {
    color = c;
    lightName = name;
    glUniform3fv(glGetUniformLocation(shader->getShaderID(), (lightName + "].color").c_str()), 1, (float*)&color);
}

DirLight::DirLight(int i, Shader* shader, glm::vec3 dir, glm::vec3 c)
    : Light("u_dir_lights[" + to_string(i), shader, c) {
    direction = glm::vec4(dir, 0.0f);
    directionLocation = glGetUniformLocation(shader->getShaderID(), (lightName + "].direction").c_str());
}

void DirLight::update(glm::mat4 view, glm::mat4 posMat) {
    direction = posMat * direction;
    glm::vec4 transformedDirection = glm::transpose(glm::inverse(view)) * direction;
    glUniform3fv(directionLocation, 1, (float*)&transformedDirection);
}

#ifdef _DEBUG
void DirLight::renderDebugUI() {
    if (ImGui::TreeNode((lightName + "]").c_str())) {
        ImGui::DragFloat3("Dir", (float*)&direction, 0.1, -FLT_MAX, +FLT_MAX);
        ImGui::TreePop();
    }
}
#endif

PointLight::PointLight(int i, Shader* shader, glm::vec3 pos, glm::vec3 c, float l, float q)
    : PointLight(i, shader, glm::vec4(pos, 1.0f), c, l, q) {}
PointLight::PointLight(int i, Shader* shader, glm::vec4 pos, glm::vec3 c, float l, float q)
    : Light("u_point_lights[" + to_string(i), shader, c) {
    position = pos;
    glUniform1f(glGetUniformLocation(shader->getShaderID(), (lightName + "].linear").c_str()), l);
    glUniform1f(glGetUniformLocation(shader->getShaderID(), (lightName + "].quadratic").c_str()), q);
    positionLocation = glGetUniformLocation(shader->getShaderID(), (lightName + "].position").c_str());
}

void PointLight::update(glm::mat4 view, glm::mat4 posMat) {
    position = posMat * position;
    glm::vec3 transformedPosition = (glm::vec3)(view * position);
    glUniform3fv(positionLocation, 1, (float*)&transformedPosition);
}

#ifdef _DEBUG
void PointLight::renderDebugUI() {
    if (ImGui::TreeNode((lightName + "]").c_str())) {
        ImGui::DragFloat3("Pos", (float*)&position, 0.1, -FLT_MAX, +FLT_MAX);
        ImGui::TreePop();
    }
}
#endif

SpotLight::SpotLight(int i, Shader* shader, glm::vec3 pos, glm::vec3 dir, glm::vec3 c, float in, float o)
    : SpotLight(i, shader, glm::vec4(pos, 1.0f), dir, c, in, o) {}
SpotLight::SpotLight(int i, Shader* shader, glm::vec4 pos, glm::vec3 dir, glm::vec3 c, float in, float o)
    : Light("u_spot_lights[" + to_string(i), shader, c) {
    position = pos;
    direction = glm::vec4(dir, 0.0f);
    glUniform1f(glGetUniformLocation(shader->getShaderID(), (lightName + "].innerCone").c_str()), in);
    glUniform1f(glGetUniformLocation(shader->getShaderID(), (lightName + "].outerCone").c_str()), o);
    positionLocation = glGetUniformLocation(shader->getShaderID(), (lightName + "].position").c_str());
    directionLocation = glGetUniformLocation(shader->getShaderID(), (lightName + "].direction").c_str());
}

void SpotLight::update(glm::mat4 view, glm::mat4 posMat) {
    position = posMat * position;
    glm::vec3 transformedPosition = (glm::vec3)(view * position);
    glUniform3fv(positionLocation, 1, (float*)&transformedPosition);

    direction = posMat * direction;
    glm::vec4 transformedDirection = glm::transpose(glm::inverse(view)) * direction;
    glUniform3fv(directionLocation, 1, (float*)&transformedDirection);
}

#ifdef _DEBUG
void SpotLight::renderDebugUI() {
    if (ImGui::TreeNode((lightName + "]").c_str())) {
        ImGui::DragFloat3("Pos", (float*)&position, 0.1, -FLT_MAX, +FLT_MAX);
        ImGui::DragFloat3("Dir", (float*)&direction, 0.1, -FLT_MAX, +FLT_MAX);
        ImGui::TreePop();
    }
}
#endif
