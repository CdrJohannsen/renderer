#pragma once

// Include is needed before including wlr-layer-shell-unstable-v1-protocol
#include "wayland-server.h"
#define namespace namespace_
#include "wlr-layer-shell-unstable-v1-protocol.h"
#undef namespace

struct wlr_layer_surface {
    struct wl_resource *resource;
    struct wl3d_surface *surface;
    class Layer *layer;

    zwlr_layer_shell_v1_layer wlr_layer;
};

struct wl_global *wlr_layer_shell_create(struct wl_display *display);
