#include "wayland/util.h"

void remove_resource(struct wl_resource *resource) { wl_list_remove(wl_resource_get_link(resource)); }
void destroy_resource(struct wl_client *client, struct wl_resource *resource) { wl_resource_destroy(resource); }
