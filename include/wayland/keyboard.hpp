#pragma once

#include <cstdint>

struct xkb {
    struct xkb_context* context;
    struct xkb_state* state;

    struct {
        struct xkb_keymap* map;
        int fd;
        uint32_t size;
        char* area;
    } keymap;

    struct {
        uint32_t ctrl, alt, super, shift;
    } indices;
};

struct wl3d_keyboard {
    struct wl_resource* resource = nullptr;

    struct wl3d_surface* focused = nullptr;

    struct xkb xkb;

    void key(uint32_t time, uint32_t key, uint32_t state);
    void enter(struct wl3d_surface* surface);
    void leave(struct wl3d_surface* surface);
};

struct wl3d_keyboard* keyboard_create(struct xkb_rule_names* names);

void keyboard_destroy(struct wl3d_keyboard* keyboard);

struct wl_resource* keyboard_bind(struct wl3d_keyboard* keyboard, struct wl_client* client, uint32_t version,
                                  uint32_t id);
