#include <wayland-server.h>
#include <iostream>

#include "geometry.h"
#include "wayland/subcompositor.h"
#include "wayland/surface.h"
#include "wayland/util.h"

void set_position(struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y) {}

void place_above(struct wl_client *client, struct wl_resource *resource, struct wl_resource *sibling) {}

void place_below(struct wl_client *client, struct wl_resource *resource, struct wl_resource *sibling) {}

void set_sync(struct wl_client *client, struct wl_resource *resource) {}

void set_desync(struct wl_client *client, struct wl_resource *resource) {}

struct wl_subsurface_interface subsurface_impl{
    .destroy = destroy_resource,
    .set_position = set_position,
    .place_above = place_above,
    .place_below = place_below,
    .set_sync = set_sync,
    .set_desync = set_desync,
};

static void destroy_subsurface(struct wl_resource *resource) {
    struct wl3d_subsurface *subsurface = static_cast<struct wl3d_subsurface *>(wl_resource_get_user_data(resource));

    delete subsurface;
}

static struct wl3d_subsurface *subsurface_new(struct wl_client *client, uint32_t version, uint32_t id,
                                              struct wl3d_surface *surface) {
    wl3d_subsurface *subsurface = new struct wl3d_subsurface();
    subsurface->resource = wl_resource_create(client, &wl_subsurface_interface, version, id);
    subsurface->surface = surface;
    wl_resource_set_implementation(subsurface->resource, &subsurface_impl, subsurface, destroy_subsurface);

    return subsurface;
}

static void get_subsurface(struct wl_client *client, struct wl_resource *resource, uint32_t id,
                           struct wl_resource *surface_resource, struct wl_resource *parent) {
    struct wl3d_subsurface *subsurface;
    struct wl3d_surface *surface = static_cast<struct wl3d_surface *>(wl_resource_get_user_data(surface_resource));

    subsurface = subsurface_new(client, wl_resource_get_version(resource), id, surface);
    if (!subsurface) wl_client_post_no_memory(client);
}

static const struct wl_subcompositor_interface subcompositor_impl = {
    .destroy = destroy_resource,
    .get_subsurface = get_subsurface,
};

static void bind_subcompositor(struct wl_client *client, void *data, uint32_t version, uint32_t id) {
    struct wl_resource *resource;

    resource = wl_resource_create(client, &wl_subcompositor_interface, version, id);
    if (!resource) {
        wl_client_post_no_memory(client);
        return;
    }
    wl_resource_set_implementation(resource, &subcompositor_impl, NULL, NULL);
}

struct wl_global *subcompositor_create(struct wl_display *display) {
    return wl_global_create(display, &wl_subcompositor_interface, 1, NULL, &bind_subcompositor);
}
