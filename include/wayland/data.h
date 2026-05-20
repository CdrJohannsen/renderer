#pragma once

#include <cstdint>

struct wl_resource *data_source_new(struct wl_client *client, uint32_t version, uint32_t id);
struct wl_resource *data_offer_new(struct wl_client *client, struct wl_resource *source, uint32_t version);
void data_send_mime_types(struct wl_resource *source, struct wl_resource *offer);
