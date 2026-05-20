#pragma once

struct wl3d_output {
    struct wl_resource* resource;
    struct wl_global* global;
};

struct wl3d_output* output_create(struct wl_display* display);

void output_destroy(struct wl3d_output* output);
