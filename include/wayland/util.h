#pragma once

#include <wayland-server.h>

void remove_resource(struct wl_resource *resource);
void destroy_resource(struct wl_client *client, struct wl_resource *resource);
