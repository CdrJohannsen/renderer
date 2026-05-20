#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include "floating_camera.hpp"
#include "shader.hpp"
#include "wayland/globals.h"
#include "wayland/keyboard.hpp"
#include "wayland/pointer.hpp"
#include "wayland/seat.h"
#include "wayland/surface.h"

static constexpr float kSurfaceScaling = 1740;

class Surface {
   public:
    Surface(struct wl3d_surface *surface) {
        surface_ = surface;
        surface_->surface_object = this;
    }

    virtual void init(Shader *shader) = 0;

    virtual void render(FloatingCamera *camera) = 0;

    virtual void commit() = 0;

    void deleteLater() {
        delete_later = true;
        // std::cout << "deleteLater" << std::endl;

        if (focused) {
            focused = false;
            globals_wl.seat->keyboard->leave(surface_);
            globals_wl.seat->pointer->leave(surface_);
        }
    }
    bool toDelete() { return delete_later; }

    virtual void close() = 0;

    virtual void configure() = 0;

   protected:
    struct wl3d_surface *surface_ = nullptr;

    bool delete_later = false;

    bool focused = false;
};
