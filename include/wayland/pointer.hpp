#pragma once

#include <cstdint>

struct wl3d_pointer {
    struct wl_resource* resource = nullptr;

    struct wl3d_surface* focused = nullptr;

    void motion(uint32_t time, double x, double y);
    void button(uint32_t time, uint32_t button, uint32_t state);
    void axis(uint32_t time, float x, float y);
    void enter(struct wl3d_surface* surface);
    void leave(struct wl3d_surface* surface);
};

struct wl3d_pointer* pointer_create();

void pointer_destroy(struct wl3d_pointer* pointer);

struct wl_resource* pointer_bind(struct wl3d_pointer* pointer, struct wl_client* client, uint32_t version, uint32_t id);
