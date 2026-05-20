#pragma once

#include <stdint.h>
#include <wayland-server-protocol.h>

struct wl3d_buffer {
    uint32_t width, height, stride;
    enum wl_shm_format format;
    uint8_t *data;
    void (*extra_destructor)(struct wl3d_buffer *buffer);
    int32_t references = 0;
};

struct wl3d_buffer *wayland_buffer_get(struct wl_resource *resource);
struct wl_resource *wayland_buffer_create_resource(struct wl_client *client, uint32_t version, uint32_t id,
                                                   struct wl3d_buffer *buffer);
