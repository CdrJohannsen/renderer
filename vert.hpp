#pragma once
#include <math.h>
#include <glm/glm.hpp>
#include <iostream>
using namespace std;

class Vert {
    public:
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 tangent;

        glm::vec2 texCoord;
};
