#pragma once
#include "shader.hpp"

class Light {
   public:
    Light(string name, Shader* shader, glm::vec3 c);

   protected:
    string lightName;
    glm::vec3 color;
};

class DirLight : public Light {
   public:
    DirLight(int i, Shader* shader, glm::vec3 dir, glm::vec3 c);

    void update(glm::mat4 view, glm::mat4 posMat);

#ifdef _DEBUG
    void renderDebugUI();
#endif

   private:
    glm::vec4 direction;
    int directionLocation;
};

class PointLight : public Light {
   public:
    PointLight(int i, Shader* shader, glm::vec4 pos, glm::vec3 c, float l, float q);
    PointLight(int i, Shader* shader, glm::vec3 pos, glm::vec3 c, float l, float q);

    void update(glm::mat4 view, glm::mat4 posMat);

#ifdef _DEBUG
    void renderDebugUI();
#endif

   private:
    glm::vec4 position;
    int positionLocation;
};

class SpotLight : public Light {
   public:
    SpotLight(int i, Shader* shader, glm::vec4 pos, glm::vec3 dir, glm::vec3 c, float in, float o);
    SpotLight(int i, Shader* shader, glm::vec3 pos, glm::vec3 dir, glm::vec3 c, float in, float o);

    void update(glm::mat4 view, glm::mat4 posMat);

#ifdef _DEBUG
    void renderDebugUI();
#endif

   private:
    glm::vec4 position;
    int positionLocation;
    glm::vec4 direction;
    int directionLocation;
};
