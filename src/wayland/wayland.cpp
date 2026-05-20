#include <wayland-server-core.h>
#include <wayland-server-protocol.h>

#include "wayland/data_device_manager.h"
#include "wayland/globals.h"
#include "wayland/output.hpp"
#include "wayland/seat.h"
#include "wayland/shm.h"
#include "wayland/subcompositor.h"
#include "wayland/surface.h"
#include "wayland/util.h"
#include "wayland/wayland.h"
#include "wayland/wlr_layer_shell.hpp"
#include "wayland/xdg-shell.h"

struct globals_wl globals_wl = {};

static void create_surface(struct wl_client *client, struct wl_resource *resource, uint32_t id) {
    struct wl3d_surface *surface;

    /* Initialize surface. */
    surface = surface_new(client, wl_resource_get_version(resource), id);

    if (!surface) {
        wl_resource_post_no_memory(resource);
        return;
    }
}

void add(struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y, int32_t width, int32_t height) {}
void subtract(struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y, int32_t width,
              int32_t height) {}

static const struct wl_region_interface region_impl = {
    .destroy = destroy_resource,
    .add = add,
    .subtract = subtract,
};

static void create_region(struct wl_client *client, struct wl_resource *resource, uint32_t id) {
    struct wl_resource *region_resource =
        wl_resource_create(client, &wl_region_interface, wl_resource_get_version(resource), id);

    if (region_resource) {
        wl_resource_set_implementation(region_resource, &region_impl, nullptr /* unused: data */,
                                       nullptr /* unused:destroy_funct */);
    } else {
        wl_resource_post_no_memory(resource);
    }
}

static const struct wl_compositor_interface compositor_impl = {
    .create_surface = create_surface,
    .create_region = create_region,
};

static void bind_compositor(struct wl_client *client, void *data, uint32_t version, uint32_t id) {
    struct wl_resource *resource;

    resource = wl_resource_create(client, &wl_compositor_interface, version, id);
    if (!resource) {
        wl_client_post_no_memory(client);
        return;
    }
    wl_resource_set_implementation(resource, &compositor_impl, NULL, NULL);
}

bool init_wayland(struct wl_display *display) {
    globals_wl.display = display;
    globals_wl.seat = seat_create(display, "seat0");
    globals_wl.data_device_manager = data_device_manager_create(display);
    globals_wl.compositor = wl_global_create(display, &wl_compositor_interface, 4, nullptr, &bind_compositor);
    if (!globals_wl.compositor) {
        return false;
    }
    struct wl_global *subcompositor = subcompositor_create(display);
    globals_wl.shm = shm_create(display);
    globals_wl.wlr_layer_shell = wlr_layer_shell_create(display);
    struct wl_global *xdg_shell = xdg_shell_create(display);
    globals_wl.output = output_create(display);

    return true;
}
