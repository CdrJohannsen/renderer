#pragma once

#include <glm/glm.hpp>
#include "ilayer_renderable.hpp"
#include "surface.hpp"

class Layer : public Surface {
   public:
    Layer(struct wlr_layer_surface *layer);
    ~Layer();

    void init(Shader *shader);

    void render(FloatingCamera *camera);

    void commit();

    void close();

    void configure();

    void loadTexture();

    void enter() { focused = true; }

   private:
    struct wlr_layer_surface *layer_ = nullptr;

    LayerRenderable *renderable = nullptr;

    glm::vec2 size{1920, 1080};

    GLuint window_texture;
};
