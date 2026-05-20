#pragma once

#include <wayland-server-core.h>

struct globals_wl {
    struct wl_display* display;
    struct wl_global* compositor;
    struct wl3d_shm* shm;
    struct wl_global* data_device_manager;
    struct wl3d_seat* seat;
    struct wl3d_output* output;
    struct wl_global* wlr_layer_shell;
};

extern struct globals_wl globals_wl;
