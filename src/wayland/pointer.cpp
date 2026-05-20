
#include "wayland/pointer.hpp"
#include <wayland-server-protocol.h>
#include <wayland-util.h>
#include <iostream>
#include "wayland/globals.h"
#include "wayland/surface.h"
#include "wayland/util.h"

void wl3d_pointer::motion(uint32_t time, double x, double y) {
    if (resource) {
        wl_fixed_t fx, fy;
        fx = wl_fixed_from_double(x);
        fy = wl_fixed_from_double(y);
        wl_pointer_send_motion(resource, time, fx, fy);
        wl_pointer_send_frame(resource);
    }
}

void wl3d_pointer::button(uint32_t time, uint32_t button, uint32_t state) {
    if (resource) {
        wl_pointer_send_button(resource, wl_display_next_serial(globals_wl.display), time, button, state);
        wl_pointer_send_frame(resource);
    }
}

void wl3d_pointer::axis(uint32_t time, float x, float y) {
    if (resource) {
        if (x != 0) {
            wl_fixed_t f = wl_fixed_from_double(x);
            wl_pointer_send_axis(resource, time, WL_POINTER_AXIS_HORIZONTAL_SCROLL, f);
        }
        if (y != 0) {
            wl_fixed_t f = wl_fixed_from_double(y);
            wl_pointer_send_axis(resource, time, WL_POINTER_AXIS_VERTICAL_SCROLL, f);
        }
        wl_pointer_send_frame(resource);
    }
}

void wl3d_pointer::enter(struct wl3d_surface* surface) {
    if (resource && (focused != surface)) {
        focused = surface;
        wl_pointer_send_enter(resource, wl_display_next_serial(globals_wl.display), surface->resource, 5, 5);
        wl_pointer_send_frame(resource);
    }
}

void wl3d_pointer::leave(struct wl3d_surface* surface) {
    if (resource && (focused == surface)) {
        focused = nullptr;
        wl_pointer_send_leave(resource, wl_display_next_serial(globals_wl.display), surface->resource);
    }
}

static void set_cursor(struct wl_client* client, struct wl_resource* resource, uint32_t serial,
                       struct wl_resource* surface, int32_t hotspot_x, int32_t hotspot_y) {}

struct wl_pointer_interface pointer_impl{
    .set_cursor = set_cursor,
    .release = destroy_resource,
};

static void unbind(struct wl_resource* resource) {
    struct wl3d_pointer* pointer = static_cast<struct wl3d_pointer*>(wl_resource_get_user_data(resource));
    // TODO
    // input_focus_remove_resource(&pointer->focus, resource);
}

struct wl_resource* pointer_bind(struct wl3d_pointer* pointer, struct wl_client* client, uint32_t version,
                                 uint32_t id) {
    pointer->resource = wl_resource_create(client, &wl_pointer_interface, version, id);
    if (!pointer->resource) {
        return nullptr;
    }

    wl_resource_set_implementation(pointer->resource, &pointer_impl, pointer, &unbind);

    // TODO
    // input_focus_add_resource(&pointer->focus, client_resource);

    return pointer->resource;
}

struct wl3d_pointer* pointer_create() {
    wl3d_pointer* pointer = new wl3d_pointer();
    return pointer;
}

void pointer_destroy(struct wl3d_pointer* pointer) { delete pointer; }
