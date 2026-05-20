
#include "wayland/keyboard.hpp"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <wayland-server-protocol.h>
#include <xkbcommon/xkbcommon.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "wayland/globals.h"
#include "wayland/surface.h"
#include "wayland/util.h"

void wl3d_keyboard::key(uint32_t time, uint32_t key, uint32_t state) {
    if (resource) {
        wl_keyboard_send_key(resource, wl_display_next_serial(globals_wl.display), time, key, state);
    }
}

void wl3d_keyboard::enter(struct wl3d_surface* surface) {
    if (resource && (focused != surface)) {
        struct wl_array keys;
        wl_array_init(&keys);
        focused = surface;
        wl_keyboard_send_enter(resource, wl_display_next_serial(globals_wl.display), surface->resource, &keys);
        // TODO
        wl_keyboard_send_modifiers(resource, wl_display_next_serial(globals_wl.display), 0, 0, 16, 0);
    }
}

void wl3d_keyboard::leave(struct wl3d_surface* surface) {
    if (resource && (focused == surface)) {
        focused = nullptr;
        wl_keyboard_send_leave(resource, wl_display_next_serial(globals_wl.display), surface->resource);
        resource = nullptr;
    }
}

struct wl_keyboard_interface keyboard_impl{
    .release = destroy_resource,
};

static void unbind(struct wl_resource* resource) {
    struct wl3d_keyboard* keyboard = static_cast<struct wl3d_keyboard*>(wl_resource_get_user_data(resource));
    // TODO
    // input_focus_remove_resource(&keyboard->focus, resource);
}

struct wl_resource* keyboard_bind(struct wl3d_keyboard* keyboard, struct wl_client* client, uint32_t version,
                                  uint32_t id) {
    keyboard->resource = wl_resource_create(client, &wl_keyboard_interface, version, id);
    if (!keyboard->resource) {
        return nullptr;
    }

    wl_resource_set_implementation(keyboard->resource, &keyboard_impl, keyboard, &unbind);

    wl_keyboard_send_keymap(keyboard->resource, WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1, keyboard->xkb.keymap.fd,
                            keyboard->xkb.keymap.size);

    // TODO
    // input_focus_add_resource(&keyboard->focus, client_resource);

    if (version >= 4) wl_keyboard_send_repeat_info(keyboard->resource, 25, 600);

    return keyboard->resource;
}

static bool update_keymap(struct xkb* xkb) {
    std::string keymap_path;
    const char* keymap_directory;
    char* keymap_string;
    int ret;

    if (!(keymap_directory = getenv("XDG_RUNTIME_DIR"))) keymap_directory = "/tmp";

    xkb->indices.ctrl = xkb_keymap_mod_get_index(xkb->keymap.map, XKB_MOD_NAME_CTRL);
    xkb->indices.alt = xkb_keymap_mod_get_index(xkb->keymap.map, XKB_MOD_NAME_ALT);
    xkb->indices.super = xkb_keymap_mod_get_index(xkb->keymap.map, XKB_MOD_NAME_LOGO);
    xkb->indices.shift = xkb_keymap_mod_get_index(xkb->keymap.map, XKB_MOD_NAME_SHIFT);

    /* In order to send the keymap to clients, we must first convert it to a
     * string and then mmap it to a file. */
    keymap_string = xkb_keymap_get_as_string(xkb->keymap.map, XKB_KEYMAP_FORMAT_TEXT_V2);

    if (!keymap_string) {
        std::cerr << "Could not get XKB keymap as a string\n";
        goto error0;
    }

    keymap_path = std::string(keymap_directory) + "/swc-xkb-keymap-XXXXXX";

    xkb->keymap.size = strlen(keymap_string) + 1;
    xkb->keymap.fd = mkostemp(keymap_path.data(), O_CLOEXEC);

    if (xkb->keymap.fd == -1) {
        std::cerr << "Could not create XKB keymap file\n";
        goto error1;
    }

    unlink(keymap_path.data());

    if (posix_fallocate(xkb->keymap.fd, 0, xkb->keymap.size) != 0 && ftruncate(xkb->keymap.fd, xkb->keymap.size) != 0) {
        std::cerr << "Could not resize XKB keymap file\n";
        goto error2;
    }

    xkb->keymap.area = (char*)mmap(NULL, xkb->keymap.size, PROT_READ | PROT_WRITE, MAP_SHARED, xkb->keymap.fd, 0);

    if (xkb->keymap.area == MAP_FAILED) {
        std::cerr << "Could not mmap XKB keymap string\n";
        goto error2;
    }

    strcpy(xkb->keymap.area, keymap_string);
    free(keymap_string);

    return true;

error2:
    close(xkb->keymap.fd);
error1:
    free(keymap_string);
error0:
    return false;
}

struct wl3d_keyboard* keyboard_create(struct xkb_rule_names* names) {
    wl3d_keyboard* keyboard = new wl3d_keyboard();

    struct xkb* xkb = &keyboard->xkb;
    if (!(xkb->context = xkb_context_new(XKB_CONTEXT_NO_FLAGS))) {
        std::cerr << "Could not create XKB context" << std::endl;
        goto error1;
    }

    if (!(xkb->keymap.map = xkb_keymap_new_from_names2(xkb->context, names, XKB_KEYMAP_FORMAT_TEXT_V2,
                                                       XKB_KEYMAP_COMPILE_NO_FLAGS))) {
        std::cerr << "Could not create XKB keymap" << std::endl;
        goto error2;
    }

    if (!(xkb->state = xkb_state_new(xkb->keymap.map))) {
        std::cerr << "Could not create XKB state" << std::endl;
        goto error3;
    }

    if (!update_keymap(xkb)) {
        std::cerr << "Could not update XKB keymap" << std::endl;
        goto error4;
    }

    return keyboard;

error4:
    xkb_state_unref(keyboard->xkb.state);
error3:
    xkb_keymap_unref(keyboard->xkb.keymap.map);
error2:
    xkb_context_unref(keyboard->xkb.context);
error1:
    free(keyboard);
    return nullptr;
}

void keyboard_destroy(struct wl3d_keyboard* keyboard) {
    munmap(keyboard->xkb.keymap.area, keyboard->xkb.keymap.size);
    close(keyboard->xkb.keymap.fd);
    xkb_state_unref(keyboard->xkb.state);
    xkb_keymap_unref(keyboard->xkb.keymap.map);
    xkb_context_unref(keyboard->xkb.context);
    delete keyboard;
}
