#pragma once

#include "xdg-shell-protocol.h"

struct xdg_surface {
    struct wl_resource *resource, *role;
    struct wl3d_surface *surface;
    struct wl_listener surface_destroy_listener, role_destroy_listener;
    uint32_t configure_serial;
    class WindowSurface *window_surface = nullptr;
};

struct xdg_toplevel {
    struct Window *window;
    struct wl_resource *resource;
    struct xdg_surface *xdg_surface;
};

struct xdg_positioner {
    int32_t width, height;
    int32_t anchor_x = 0;
    int32_t anchor_y = 0;
    int32_t anchor_width, anchor_height;
    enum xdg_positioner_anchor anchor;
    enum xdg_positioner_gravity gravity;
    enum xdg_positioner_constraint_adjustment constraint;
    int32_t offset_x, offset_y = 0;
};

struct xdg_popup {
    struct wl_resource *resource;
    struct xdg_surface *xdg_surface;
    struct xdg_positioner positioner;
    struct xdg_surface *parent;
    struct Popup *popup;
};

struct wl_global *xdg_shell_create(struct wl_display *display);
