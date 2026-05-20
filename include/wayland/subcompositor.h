#pragma once

#include <wayland-server.h>

struct wl3d_subsurface {
    struct wl_resource *resource;
    struct wl3d_surface *surface;
};

struct wl_global *subcompositor_create(struct wl_display *display);
