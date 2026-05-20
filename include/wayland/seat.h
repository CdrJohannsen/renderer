
#pragma once

#include <wayland-util.h>
#include <xkbcommon/xkbcommon.h>
#include <cstdint>
#include <string>
struct wl_display;

struct wl3d_seat {
    std::string name;
    struct wl_global *global;
    struct wl3d_pointer *pointer;
    struct wl3d_keyboard *keyboard;
    struct data_device *data_device;
    struct wl_list data_devices;
    uint32_t capabilities = 0;

    struct xkb_rule_names names;
};

struct wl3d_seat *seat_create(struct wl_display *display, const char *name);
void seat_destroy(struct wl3d_seat *seat);
