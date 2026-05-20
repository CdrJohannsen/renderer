#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class LayerRenderable {
   public:
    virtual void init() = 0;

    virtual void render(GLuint texture) = 0;

    virtual void setSize(glm::vec2 size) { size_ = size; }

   protected:
    GLuint VAO = 0;

    glm::vec2 size_{200, 200};
};
