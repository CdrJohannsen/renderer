#pragma once

#include <wayland-server.h>

enum { DATA_DEVICE_EVENT_SELECTION_CHANGED };

struct data_device {
    /* The data source corresponding to the current selection. */
    struct wl_resource *selection;
    struct wl_listener selection_destroy_listener;

    struct wl_signal event_signal;
    struct wl_list resources;
};

struct data_device *data_device_create(void);
void data_device_destroy(struct data_device *data_device);
struct wl_resource *data_device_bind(struct wl3d_seat *seat, struct wl_client *client, uint32_t version, uint32_t id);
void data_device_offer_selection(struct data_device *data_device, struct wl_client *client);
