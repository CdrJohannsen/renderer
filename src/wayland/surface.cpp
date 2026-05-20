#include "wayland/surface.h"
#include <iostream>
#include "globals.h"
#include "wayland/util.h"

static void state_initialize(struct surface_state *state) {
    state->buffer = nullptr;

    state->is_damaged = false;

    wl_list_init(&state->frame_callbacks);
}

static void state_finalize(struct surface_state *state) {
    struct wl_resource *resource, *tmp;

    /* Remove all leftover callbacks. */
    wl_list_for_each_safe(resource, tmp, &state->frame_callbacks, link) wl_resource_destroy(resource);
}

static void state_set_buffer(struct surface_state *state, struct wl_resource *resource) {
    struct wl3d_buffer *buffer = resource ? wayland_buffer_get(resource) : NULL;

    state->buffer = buffer;
    state->buffer_resource = resource;
}

static void attach(struct wl_client *client, struct wl_resource *resource, struct wl_resource *buffer_resource,
                   int32_t x, int32_t y) {
    struct wl3d_surface *surface = static_cast<struct wl3d_surface *>(wl_resource_get_user_data(resource));

    surface->pending.commit.attach = true;

    state_set_buffer(&surface->pending.state, buffer_resource);
    surface->pending.x = x;
    surface->pending.y = y;
}

static void damage(struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y, int32_t width,
                   int32_t height) {
    struct wl3d_surface *surface = static_cast<struct wl3d_surface *>(wl_resource_get_user_data(resource));

    surface->pending.commit.damage = true;
    surface->pending.state.is_damaged = true;
}

static void frame(struct wl_client *client, struct wl_resource *resource, uint32_t callback) {
    struct wl3d_surface *surface = static_cast<struct wl3d_surface *>(wl_resource_get_user_data(resource));

    struct wl_resource *callback_resource;

    callback_resource = wl_resource_create(client, &wl_callback_interface, 1, callback);
    if (!callback_resource) {
        wl_resource_post_no_memory(resource);
        return;
    }
    surface->pending.commit.frame = true;
    wl_resource_set_implementation(callback_resource, NULL, NULL, &remove_resource);
    wl_list_insert(surface->pending.state.frame_callbacks.prev, wl_resource_get_link(callback_resource));
}

static void set_opaque_region(struct wl_client *client, struct wl_resource *resource,
                              struct wl_resource *region_resource) {}

static void set_input_region(struct wl_client *client, struct wl_resource *resource,
                             struct wl_resource *region_resource) {}

static void commit(struct wl_client *client, struct wl_resource *resource) {
    struct wl3d_surface *surface = static_cast<struct wl3d_surface *>(wl_resource_get_user_data(resource));
    struct wl3d_buffer *buffer;

    if (surface->pending.commit.attach) {
        if (surface->state.buffer && (surface->state.buffer != surface->pending.state.buffer)) {
            wl_buffer_send_release(surface->state.buffer_resource);
        }
        state_set_buffer(&surface->state, surface->pending.state.buffer_resource);
        surface->pending.commit.attach = false;
    }

    buffer = surface->state.buffer;

    if (surface->pending.commit.damage) {
        surface->state.is_damaged = surface->pending.state.is_damaged;
        surface->pending.commit.damage = false;
    }

    if (surface->pending.commit.frame) {
        wl_list_insert_list(&surface->state.frame_callbacks, &surface->pending.state.frame_callbacks);
        wl_list_init(&surface->pending.state.frame_callbacks);
        surface->pending.commit.frame = false;
    }

    if (buffer && surface->surface_object) {
        surface->surface_object->commit();
    }
}

static void set_buffer_transform(struct wl_client *client, struct wl_resource *surface, int32_t transform) {
    if (transform != WL_OUTPUT_TRANSFORM_NORMAL) {
        wl_resource_post_error(surface, WL_SURFACE_ERROR_INVALID_TRANSFORM,
                               "buffer transform %" PRId32 " not supported", transform);
    }
}

static void set_buffer_scale(struct wl_client *client, struct wl_resource *surface, int32_t scale) {
    if (scale != 1) wl_resource_post_error(surface, WL_SURFACE_ERROR_INVALID_SCALE, "buffer scale not supported");
}

static void damage_buffer(struct wl_client *client, struct wl_resource *surface, int32_t x, int32_t y, int32_t w,
                          int32_t h) {
    damage(client, surface, x, y, w, h);
}

static const struct wl_surface_interface surface_impl = {
    .destroy = destroy_resource,
    .attach = attach,
    .damage = damage,
    .frame = frame,
    .set_opaque_region = set_opaque_region,
    .set_input_region = set_input_region,
    .commit = commit,
    .set_buffer_transform = set_buffer_transform,
    .set_buffer_scale = set_buffer_scale,
    .damage_buffer = damage_buffer,
};

static void surface_destroy(struct wl_resource *resource) {
    struct wl3d_surface *surface = static_cast<struct wl3d_surface *>(wl_resource_get_user_data(resource));
    // std::cout << "surface_destroy" << std::endl;

    state_finalize(&surface->state);
    state_finalize(&surface->pending.state);

    if (surface->surface_object) surface->surface_object->deleteLater();

    delete surface;
}

struct wl3d_surface *surface_new(struct wl_client *client, uint32_t version, uint32_t id) {
    struct wl3d_surface *surface;

    surface = new struct wl3d_surface();
    if (!surface) {
        return nullptr;
    }

    surface->resource = wl_resource_create(client, &wl_surface_interface, version, id);
    if (!surface->resource) {
        delete surface;
        return nullptr;
    }

    wl_resource_set_implementation(surface->resource, &surface_impl, surface, &surface_destroy);

    surface->pending.commit.attach = false;
    surface->pending.commit.damage = false;
    surface->pending.commit.frame = false;

    state_initialize(&surface->state);
    state_initialize(&surface->pending.state);

    return surface;
}
