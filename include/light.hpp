#pragma once
#include "shader.hpp"

class Light {
   public:
    Light(int i, glm::vec3 d, glm::vec3 s, glm::vec3 a);

    Light(int i, glm::vec3 d, float a);

   protected:
    string index;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 ambient;
};

class DirLight : public Light {
   public:
    DirLight(int i, Shader* shader, glm::vec3 dir, glm::vec3 d, float a);
    DirLight(int i, Shader* shader, glm::vec3 dir, glm::vec3 d, glm::vec3 s, glm::vec3 a);

    void update(glm::mat4 view, glm::mat4 posMat);

   private:
    glm::vec4 direction;
    int directionLocation;
};

class PointLight : public Light {
   public:
    PointLight(int i, Shader* shader, glm::vec4 pos, glm::vec3 d, float a, float l, float q);
    PointLight(int i, Shader* shader, glm::vec3 pos, glm::vec3 d, glm::vec3 s, glm::vec3 a, float l, float q);
    PointLight(int i, Shader* shader, glm::vec4 pos, glm::vec3 d, glm::vec3 s, glm::vec3 a, float l, float q);

    void update(glm::mat4 view, glm::mat4 posMat);

   private:
    glm::vec4 position;
    int positionLocation;
};

class SpotLight : public Light {
   public:
    SpotLight(int i, Shader* shader, glm::vec4 pos, glm::vec3 dir, glm::vec3 d, float a, float in, float o);
    SpotLight(int i, Shader* shader, glm::vec3 pos, glm::vec3 dir, glm::vec3 d, glm::vec3 s, glm::vec3 a, float in,
              float o);
    SpotLight(int i, Shader* shader, glm::vec4 pos, glm::vec3 dir, glm::vec3 d, glm::vec3 s, glm::vec3 a, float in,
              float o);

    void update(glm::mat4 view, glm::mat4 posMat);

   private:
    glm::vec4 position;
    int positionLocation;
    glm::vec4 direction;
    int directionLocation;
};
