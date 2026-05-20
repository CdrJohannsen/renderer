#include <wayland-server.h>
#include <iostream>

#include "wayland/buffer.h"
#include "wayland/util.h"

static const struct wl_buffer_interface buffer_impl = {
    .destroy = destroy_resource,
};

struct wl3d_buffer *wayland_buffer_get(struct wl_resource *resource) {
    if (wl_resource_instance_of(resource, &wl_buffer_interface, &buffer_impl))
        return static_cast<struct wl3d_buffer *>(wl_resource_get_user_data(resource));

    return nullptr;
}

static void destroy_buffer(struct wl_resource *resource) {
    struct wl3d_buffer *buffer = static_cast<struct wl3d_buffer *>(wl_resource_get_user_data(resource));
    buffer->references--;
    if (buffer->references <= 0) {
        // TODO: Destructor
        // std::cout << "destroy_buffer" << std::endl;
    }
}

struct wl_resource *wayland_buffer_create_resource(struct wl_client *client, uint32_t version, uint32_t id,
                                                   struct wl3d_buffer *buffer) {
    struct wl_resource *resource;

    resource = wl_resource_create(client, &wl_buffer_interface, version, id);
    if (resource) wl_resource_set_implementation(resource, &buffer_impl, buffer, &destroy_buffer);
    return resource;
}
