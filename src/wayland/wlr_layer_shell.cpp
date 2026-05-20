
#include "wayland/wlr_layer_shell.hpp"
#include <iostream>
#include "globals.h"
#include "layer.hpp"
#include "wayland/globals.h"
#include "wayland/keyboard.hpp"
#include "wayland/seat.h"
#include "wayland/util.h"

void set_size(struct wl_client *client, struct wl_resource *resource, uint32_t width, uint32_t height) {}

void set_anchor(struct wl_client *client, struct wl_resource *resource, uint32_t anchor) {}

void set_exclusive_zone(struct wl_client *client, struct wl_resource *resource, int32_t zone) {}

void set_margin(struct wl_client *client, struct wl_resource *resource, int32_t top, int32_t right, int32_t bottom,
                int32_t left) {}

void set_keyboard_interactivity(struct wl_client *client, struct wl_resource *resource,
                                uint32_t keyboard_interactivity) {
    struct wlr_layer_surface *layer_surface =
        static_cast<struct wlr_layer_surface *>(wl_resource_get_user_data(resource));

    if (keyboard_interactivity >= ZWLR_LAYER_SURFACE_V1_KEYBOARD_INTERACTIVITY_EXCLUSIVE) {
        globals_wl.seat->keyboard->enter(layer_surface->surface);
        layer_surface->layer->enter();
    }
}

void get_popup(struct wl_client *client, struct wl_resource *resource, struct wl_resource *popup) {}

void ack_configure(struct wl_client *client, struct wl_resource *resource, uint32_t serial) {}

void set_layer(struct wl_client *client, struct wl_resource *resource, uint32_t layer) {
    std::cout << "set_layer" << std::endl;
}

void set_exclusive_edge(struct wl_client *client, struct wl_resource *resource, uint32_t edge) {}

static const struct zwlr_layer_surface_v1_interface layer_surface_impl{
    .set_size = set_size,
    .set_anchor = set_anchor,
    .set_exclusive_zone = set_exclusive_zone,
    .set_margin = set_margin,
    .set_keyboard_interactivity = set_keyboard_interactivity,
    .get_popup = get_popup,
    .ack_configure = ack_configure,
    .destroy = destroy_resource,
    .set_layer = set_layer,
    .set_exclusive_edge = set_exclusive_edge,
};

void destroy_layer_surface(struct wl_resource *resource) {
    struct wlr_layer_surface *layer_surface =
        static_cast<struct wlr_layer_surface *>(wl_resource_get_user_data(resource));
    delete layer_surface;
}

void get_layer_surface(struct wl_client *client, struct wl_resource *resource, uint32_t id,
                       struct wl_resource *surface_resource, struct wl_resource *output, uint32_t layer,
                       const char *namespace_) {
    std::cout << "Layer: " << layer << std::endl;
    struct wlr_layer_surface *layer_surface = new struct wlr_layer_surface();
    if (!layer_surface) goto error0;
    layer_surface->resource =
        wl_resource_create(client, &zwlr_layer_surface_v1_interface, wl_resource_get_version(resource), id);
    if (!layer_surface->resource) goto error1;

    layer_surface->surface = static_cast<struct wl3d_surface *>(wl_resource_get_user_data(surface_resource));
    layer_surface->wlr_layer = static_cast<zwlr_layer_shell_v1_layer>(layer);
    // TODO view
    layer_surface->layer = new Layer(layer_surface);
    if (layer == ZWLR_LAYER_SHELL_V1_LAYER_BACKGROUND) {
        rendererAddSkybox(layer_surface->layer);
    } else if (layer == ZWLR_LAYER_SHELL_V1_LAYER_OVERLAY) {
        rendererAddOverlay(layer_surface->layer);
    }

    wl_resource_set_implementation(layer_surface->resource, &layer_surface_impl, layer_surface, destroy_layer_surface);

    return;

error1:
    delete layer_surface;
error0:
    wl_client_post_no_memory(client);
}

static const struct zwlr_layer_shell_v1_interface layer_shell_impl{
    .get_layer_surface = get_layer_surface,
    .destroy = destroy_resource,
};

static void bind_layer_shell(struct wl_client *client, void *data, uint32_t version, uint32_t id) {
    struct wl_resource *resource;

    resource = wl_resource_create(client, &zwlr_layer_shell_v1_interface, version, id);
    if (!resource) {
        wl_client_post_no_memory(client);
        return;
    }
    wl_resource_set_implementation(resource, &layer_shell_impl, NULL, NULL);
}

struct wl_global *wlr_layer_shell_create(struct wl_display *display) {
    return wl_global_create(display, &zwlr_layer_shell_v1_interface, 5, NULL, &bind_layer_shell);
}
