#pragma once

struct wl3d_shm {
    struct wl_global *global;
};

struct wl3d_shm *shm_create(struct wl_display *display);
void shm_destroy(struct wl3d_shm *shm);
