#pragma once
#include <glm/glm.hpp>

struct Vert {
   public:
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;

    glm::vec2 texCoord;
};
