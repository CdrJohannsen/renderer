#include "wayland/seat.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wayland-server-core.h>
#include <wayland-server-protocol.h>
#include <wayland-util.h>
#include <iostream>
#include "wayland/data_device.h"
#include "wayland/keyboard.hpp"
#include "wayland/pointer.hpp"
#include "wayland/util.h"

void get_pointer(struct wl_client *client, struct wl_resource *resource, uint32_t id) {
    struct wl3d_seat *seat = static_cast<struct wl3d_seat *>(wl_resource_get_user_data(resource));

    if (!pointer_bind(seat->pointer, client, wl_resource_get_version(resource), id)) {
        wl_resource_post_no_memory(resource);
    }
}

void get_keyboard(struct wl_client *client, struct wl_resource *resource, uint32_t id) {
    struct wl3d_seat *seat = static_cast<struct wl3d_seat *>(wl_resource_get_user_data(resource));

    if (!keyboard_bind(seat->keyboard, client, wl_resource_get_version(resource), id)) {
        wl_resource_post_no_memory(resource);
    }
}

void get_touch(struct wl_client *client, struct wl_resource *resource, uint32_t id) {
    wl_resource_post_error(resource, WL_SEAT_ERROR_MISSING_CAPABILITY, "touch is not supported");
}

void release(struct wl_client *client, struct wl_resource *resource) {
    // TODO
}

struct wl_seat_interface seat_impl{
    .get_pointer = get_pointer,
    .get_keyboard = get_keyboard,
    .get_touch = get_touch,
    .release = release,
};

static void bind_seat(struct wl_client *client, void *data, uint32_t version, uint32_t id) {
    struct wl3d_seat *seat = static_cast<struct wl3d_seat *>(data);
    struct wl_resource *resource;

    resource = wl_resource_create(client, &wl_seat_interface, version, id);
    if (!resource) {
        wl_client_post_no_memory(client);
        return;
    }
    wl_resource_set_implementation(resource, &seat_impl, seat, [](struct wl_resource *resource) {
        struct wl3d_seat *seat = static_cast<struct wl3d_seat *>(wl_resource_get_user_data(resource));
        seat->keyboard->focused = nullptr;
        seat->pointer->focused = nullptr;
        std::cout << "Removing seat " << wl_resource_get_user_data(resource) << std::endl;
        // remove_resource(resource);
    });

    if (version >= 2) wl_seat_send_name(resource, seat->name.c_str());

    seat->capabilities = WL_SEAT_CAPABILITY_KEYBOARD | WL_SEAT_CAPABILITY_POINTER;
    wl_seat_send_capabilities(resource, seat->capabilities);
}

struct wl3d_seat *seat_create(struct wl_display *display, const char *seat_name) {
    struct wl3d_seat *seat = new wl3d_seat();

    seat->names.layout = "de";
    seat->names.variant = "nodeadkeys";
    seat->names.options = "lv3:caps_switch,compose:ralt,nbsp:zwnj3zwj4,keypad:oss,kpdl:commaoss";

    if (!seat) goto error0;
    seat->name = seat_name;
    if (seat->name.empty()) {
        std::cerr << "Could not allocate seat name string" << std::endl;
        goto error1;
    }
    seat->global = wl_global_create(display, &wl_seat_interface, 10, seat, &bind_seat);
    if (!seat->global) goto error2;

    wl_list_init(&seat->data_devices);
    seat->data_device = data_device_create();
    if (!seat->data_device) {
        std::cerr << "Could not initialize data device" << std::endl;
        goto error3;
    }

    seat->keyboard = keyboard_create(&seat->names);
    if (!seat->keyboard) {
        std::cerr << "Could not initialize keyboard" << std::endl;
        goto error4;
    }

    seat->pointer = pointer_create();
    if (!seat->pointer) {
        std::cerr << "Could not initialize pointer" << std::endl;
        goto error5;
    }

    return seat;

error6:
    pointer_destroy(seat->pointer);
error5:
    keyboard_destroy(seat->keyboard);
error4:
    data_device_destroy(seat->data_device);
error3:
    wl_global_destroy(seat->global);
error2:
error1:
    delete seat;
error0:
    return NULL;
}

void seat_destroy(struct wl3d_seat *seat) {
    pointer_destroy(seat->pointer);
    keyboard_destroy(seat->keyboard);
    data_device_destroy(seat->data_device);

    wl_global_destroy(seat->global);
    delete seat;
}
