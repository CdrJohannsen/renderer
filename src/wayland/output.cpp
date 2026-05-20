
#include "wayland/output.hpp"
#include <wayland-server-protocol.h>
#include "wayland/util.h"

struct wl_output_interface output_impl{
    .release = destroy_resource,
};

static void unbind(struct wl_resource* resource) {
    struct wl3d_output* output = static_cast<struct wl3d_output*>(wl_resource_get_user_data(resource));
}

static void output_bind(struct wl_client* client, void* data, uint32_t version, uint32_t id) {
    struct wl3d_output* output = static_cast<struct wl3d_output*>(data);
    output->resource = wl_resource_create(client, &wl_output_interface, version, id);
    if (!output->resource) {
        wl_resource_post_no_memory(output->resource);
        return;
    }

    wl_resource_set_implementation(output->resource, &output_impl, output, &unbind);
    wl_output_send_name(output->resource, "WL-1");
    wl_output_send_description(output->resource, "3D Output Virtual Out (WL-1)");
    wl_output_send_scale(output->resource, 1);
    wl_output_send_mode(output->resource, WL_OUTPUT_MODE_CURRENT, 1920, 1080, 0);
    wl_output_send_geometry(output->resource, 0, 0, 480, 270, WL_OUTPUT_SUBPIXEL_UNKNOWN, "3D Output", "Virtual Out",
                            WL_OUTPUT_TRANSFORM_NORMAL);
    wl_output_send_done(output->resource);
}

struct wl3d_output* output_create(struct wl_display* display) {
    wl3d_output* output = new wl3d_output();

    output->global = wl_global_create(display, &wl_output_interface, 4, output, &output_bind);

    return output;
}

void output_destroy(struct wl3d_output* output) { delete output; }
