#pragma once

#include <stdint.h>
#include "wayland/buffer.h"

struct surface_state {
    struct wl3d_buffer *buffer = nullptr;
    struct wl_resource *buffer_resource;
    // TODO: buffer_destroy_listener ?
    bool is_damaged;
    struct wl_list frame_callbacks;
};

struct wl3d_surface {
    struct wl_resource *resource;

    struct surface_state state;

    struct {
        struct surface_state state;
        struct [[gnu::packed]] {
            bool attach : 1 = false;
            bool damage : 1 = false;
            bool frame : 1 = false;
        } commit;
        int32_t x, y;
    } pending;

    class Surface *surface_object;
};

struct wl3d_surface *surface_new(struct wl_client *client, uint32_t version, uint32_t id);
