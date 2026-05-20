
#include <SDL3/SDL_timer.h>
#include <stb/stb_image.h>
#include <wayland-server-core.h>
#include <wayland-server-protocol.h>
#include <wayland-util.h>
#include "globals.h"
#include "imgui.h"
#include "overlay.hpp"
#include "skybox.hpp"
#include "wayland/globals.h"
#include "wayland/keyboard.hpp"
#include "wayland/output.hpp"
#include "wayland/pointer.hpp"
#include "wayland/seat.h"
#include "wayland/surface.h"
#include "wayland/wlr_layer_shell.hpp"

#include "layer.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>

Layer::Layer(struct wlr_layer_surface *layer) : Surface(layer->surface) {
    layer_ = layer;

    if (layer_->wlr_layer == ZWLR_LAYER_SHELL_V1_LAYER_BACKGROUND) {
        renderable = new Skybox();
    } else if (layer_->wlr_layer == ZWLR_LAYER_SHELL_V1_LAYER_OVERLAY) {
        renderable = new Overlay();
    }

    configure();
}

Layer::~Layer() {}

void Layer::init(Shader *shader) {
    glGenTextures(1, &window_texture);
    renderable->init();
}

void Layer::render(FloatingCamera *camera) {
    renderable->render(window_texture);

    struct wl_resource *resource, *tmp;
    wl_list_for_each_safe(resource, tmp, &surface_->state.frame_callbacks, link) {
        wl_callback_send_done(resource, SDL_GetTicks());
        wl_resource_destroy(resource);
    }
}

void Layer::commit() {
    if (delete_later || (!surface_->state.buffer) || (!surface_->state.buffer_resource)) return;

    loadTexture();
}

void Layer::close() {
    zwlr_layer_surface_v1_send_closed(layer_->resource);
    if (focused) {
        focused = false;
        globals_wl.seat->keyboard->leave(surface_);
        globals_wl.seat->pointer->leave(surface_);
    }
    if (layer_->wlr_layer == ZWLR_LAYER_SHELL_V1_LAYER_BACKGROUND) {
        rendererRemoveSkybox(this);
    } else if (layer_->wlr_layer == ZWLR_LAYER_SHELL_V1_LAYER_OVERLAY) {
        rendererRemoveOverlay(this);
    }
}

void Layer::configure() {
    zwlr_layer_surface_v1_send_configure(layer_->resource, wl_display_next_serial(globals_wl.display), size.x, size.y);
}

void Layer::loadTexture() {
    glBindTexture(GL_TEXTURE_2D, window_texture);
    stbi_set_flip_vertically_on_load(layer_->wlr_layer == ZWLR_LAYER_SHELL_V1_LAYER_BACKGROUND);

    // TODO different color formats
    if ((surface_->state.buffer->height > 0) && (surface_->state.buffer->width > 0)) {
        size = {surface_->state.buffer->width, surface_->state.buffer->height};
        renderable->setSize(size);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA, surface_->state.buffer->width, surface_->state.buffer->height, 0,
                     GL_BGRA, GL_UNSIGNED_BYTE, surface_->state.buffer->data);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}
