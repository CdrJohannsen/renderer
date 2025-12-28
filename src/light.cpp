#include "light.hpp"

#include <glm/glm.hpp>

#include "shader.hpp"

#ifdef _DEBUG
#include "imgui.h"
#endif

Light::Light(string name, Shader* shader, glm::vec3 d, glm::vec3 s) {
    diffuse = d;
    specular = s;
    lightName = name;
    glUniform3fv(glGetUniformLocation(shader->getShaderID(), (lightName + "].diffuse").c_str()), 1, (float*)&diffuse);
    glUniform3fv(glGetUniformLocation(shader->getShaderID(), (lightName + "].specular").c_str()), 1, (float*)&specular);
}

Light::Light(string name, Shader* shader, glm::vec3 d) : Light(name, shader, d, d) {}

DirLight::DirLight(int i, Shader* shader, glm::vec3 dir, glm::vec3 d) : DirLight(i, shader, dir, d, d) {}
DirLight::DirLight(int i, Shader* shader, glm::vec3 dir, glm::vec3 d, glm::vec3 s)
    : Light("u_dir_lights[" + to_string(i), shader, d, s) {
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

PointLight::PointLight(int i, Shader* shader, glm::vec4 pos, glm::vec3 d, float l, float q)
    : PointLight(i, shader, pos, d, d, l, q) {}
PointLight::PointLight(int i, Shader* shader, glm::vec3 pos, glm::vec3 d, glm::vec3 s, float l, float q)
    : PointLight(i, shader, glm::vec4(pos, 1.0f), d, s, l, q) {}
PointLight::PointLight(int i, Shader* shader, glm::vec4 pos, glm::vec3 d, glm::vec3 s, float l, float q)
    : Light("u_point_lights[" + to_string(i), shader, d, s) {
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

SpotLight::SpotLight(int i, Shader* shader, glm::vec4 pos, glm::vec3 dir, glm::vec3 d, float in, float o)
    : SpotLight(i, shader, pos, dir, d, d, in, o) {}
SpotLight::SpotLight(int i, Shader* shader, glm::vec3 pos, glm::vec3 dir, glm::vec3 d, glm::vec3 s, float in, float o)
    : SpotLight(i, shader, glm::vec4(pos, 1.0f), dir, d, s, in, o) {}
SpotLight::SpotLight(int i, Shader* shader, glm::vec4 pos, glm::vec3 dir, glm::vec3 d, glm::vec3 s, float in, float o)
    : Light("u_spot_lights[" + to_string(i), shader, d, s) {
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
