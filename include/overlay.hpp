#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "ilayer_renderable.hpp"

class Overlay : public LayerRenderable {
   public:
    void init() override;

    void render(GLuint texture) override;

   private:
    void loadOverlayVAO();
};
