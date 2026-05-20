#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "ilayer_renderable.hpp"

class Skybox : public LayerRenderable {
   public:
    void init() override;

    void render(GLuint texture) override;

   private:
    void loadSkyboxVAO();
};
