/*
 * largely based on swc: libswc/xdg_shell.c
 *
 * Copyright (c) 2014, 2018 Michael Forney
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <wayland-server.h>
#include <iostream>

#include "geometry.h"
#include "globals.h"
#include "popup.hpp"
#include "wayland/globals.h"
#include "wayland/surface.h"
#include "wayland/util.h"
#include "wayland/xdg-shell.h"
#include "window.hpp"
#include "xdg-shell-protocol.h"

/* xdg_positioner */
static void destroy_positioner(struct wl_resource *resource) {
    struct xdg_positioner *positioner = static_cast<struct xdg_positioner *>(wl_resource_get_user_data(resource));

    delete positioner;
}

static void set_size(struct wl_client *client, struct wl_resource *resource, int32_t width, int32_t height) {
    struct xdg_positioner *positioner = static_cast<struct xdg_positioner *>(wl_resource_get_user_data(resource));

    if (width <= 0 || height <= 0) {
        wl_resource_post_error(resource, XDG_POSITIONER_ERROR_INVALID_INPUT, "invalid size");
        return;
    }
    positioner->width = width;
    positioner->height = height;
}

static void set_anchor_rect(struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y, int32_t width,
                            int32_t height) {
    struct xdg_positioner *positioner = static_cast<struct xdg_positioner *>(wl_resource_get_user_data(resource));

    if (width <= 0 || height <= 0) {
        wl_resource_post_error(resource, XDG_POSITIONER_ERROR_INVALID_INPUT, "invalid anchor size");
        return;
    }
    positioner->anchor_x = x;
    positioner->anchor_y = y;
    positioner->anchor_width = width;
    positioner->anchor_height = height;
    // std::cout << "x: " << positioner->anchor_x << " rect " << positioner->offset_x << std::endl;
    // std::cout << "y: " << positioner->anchor_y << " rect " << positioner->offset_y << std::endl;
}

static void set_anchor(struct wl_client *client, struct wl_resource *resource, uint32_t anchor) {
    // std::cout << "anchor: " << anchor << std::endl;
    // struct xdg_positioner *positioner = static_cast<struct xdg_positioner *>(wl_resource_get_user_data(resource));
    //
    // positioner->anchor = anchor;
}

static void set_gravity(struct wl_client *client, struct wl_resource *resource, uint32_t gravity) {
    // std::cout << "gravity: " << gravity << std::endl;
    // struct xdg_positioner *positioner = static_cast<struct xdg_positioner *>(wl_resource_get_user_data(resource));
    //
    // positioner->gravity = gravity;
}

static void set_constraint_adjustment(struct wl_client *client, struct wl_resource *resource, uint32_t constraint) {
    // struct xdg_positioner *positioner = static_cast<struct xdg_positioner *>(wl_resource_get_user_data(resource));
    //
    // positioner->constraint = constraint;
}

static void set_offset(struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y) {
    struct xdg_positioner *positioner = static_cast<struct xdg_positioner *>(wl_resource_get_user_data(resource));

    positioner->offset_x = x;
    positioner->offset_y = y;
}

static void set_reactive(struct wl_client *client, struct wl_resource *resource) {
    // struct xdg_positioner *positioner = static_cast<struct xdg_positioner *>(wl_resource_get_user_data(resource));
}

static void set_parent_size(struct wl_client *client, struct wl_resource *resource, int32_t parent_width,
                            int32_t parent_height) {
    // struct xdg_positioner *positioner = static_cast<struct xdg_positioner *>(wl_resource_get_user_data(resource));
}

static void set_parent_configure(struct wl_client *client, struct wl_resource *resource, uint32_t serial) {
    // struct xdg_positioner *positioner = static_cast<struct xdg_positioner *>(wl_resource_get_user_data(resource));
}

static const struct xdg_positioner_interface positioner_impl = {
    .destroy = destroy_resource,
    .set_size = set_size,
    .set_anchor_rect = set_anchor_rect,
    .set_anchor = set_anchor,
    .set_gravity = set_gravity,
    .set_constraint_adjustment = set_constraint_adjustment,
    .set_offset = set_offset,
    .set_reactive = set_reactive,
    .set_parent_size = set_parent_size,
    .set_parent_configure = set_parent_configure,
};

/* xdg_toplevel */
static void destroy_toplevel(struct wl_resource *resource) {
    struct xdg_toplevel *toplevel = static_cast<struct xdg_toplevel *>(wl_resource_get_user_data(resource));

    // window_finalize(&toplevel->window);
    free(toplevel);
}

static void set_parent(struct wl_client *client, struct wl_resource *resource, struct wl_resource *parent_resource) {
    // struct xdg_toplevel *toplevel = wl_resource_get_user_data(resource), *parent = NULL;
    //
    // if (parent_resource) parent = wl_resource_get_user_data(parent_resource);
    // window_set_parent(&toplevel->window, parent ? &parent->window : NULL);
}

static void set_title(struct wl_client *client, struct wl_resource *resource, const char *title) {
    struct xdg_toplevel *toplevel = static_cast<struct xdg_toplevel *>(wl_resource_get_user_data(resource));
    toplevel->window->setTitle(title);
}

static void set_app_id(struct wl_client *client, struct wl_resource *resource, const char *app_id) {
    struct xdg_toplevel *toplevel = static_cast<struct xdg_toplevel *>(wl_resource_get_user_data(resource));
    toplevel->window->setAppID(app_id);
}

static void show_window_menu(struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat,
                             uint32_t serial, int32_t x, int32_t y) {}

static void move(struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat, uint32_t serial) {
    // struct xdg_toplevel *toplevel = wl_resource_get_user_data(resource);
    // struct button *button;
    //
    // button = pointer_get_button(swc.seat->pointer, serial);
    // if (button) window_begin_move(&toplevel->window, button);
}

static void resize(struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat, uint32_t serial,
                   uint32_t edges) {
    // struct xdg_toplevel *toplevel = wl_resource_get_user_data(resource);
    // struct button *button;
    //
    // button = pointer_get_button(swc.seat->pointer, serial);
    // if (button) window_begin_resize(&toplevel->window, edges, button);
    // TODO ?
}

static void set_max_size(struct wl_client *client, struct wl_resource *resource, int32_t width, int32_t height) {
    struct xdg_toplevel *toplevel = static_cast<struct xdg_toplevel *>(wl_resource_get_user_data(resource));
    toplevel->window->setMaxSize(glm::vec2(width, height));
}

static void set_min_size(struct wl_client *client, struct wl_resource *resource, int32_t width, int32_t height) {
    struct xdg_toplevel *toplevel = static_cast<struct xdg_toplevel *>(wl_resource_get_user_data(resource));
    toplevel->window->setMinSize(glm::vec2(width, height));
}

static void set_maximized(struct wl_client *client, struct wl_resource *resource) {}

static void unset_maximized(struct wl_client *client, struct wl_resource *resource) {}

static void set_fullscreen(struct wl_client *client, struct wl_resource *resource, struct wl_resource *output) {}

static void unset_fullscreen(struct wl_client *client, struct wl_resource *resource) {}

static void set_minimized(struct wl_client *client, struct wl_resource *resource) {}

static const struct xdg_toplevel_interface toplevel_impl = {
    .destroy = destroy_resource,
    .set_parent = set_parent,
    .set_title = set_title,
    .set_app_id = set_app_id,
    .show_window_menu = show_window_menu,
    .move = move,
    .resize = resize,
    .set_max_size = set_max_size,
    .set_min_size = set_min_size,
    .set_maximized = set_maximized,
    .unset_maximized = unset_maximized,
    .set_fullscreen = set_fullscreen,
    .unset_fullscreen = unset_fullscreen,
    .set_minimized = set_minimized,
};

static struct xdg_toplevel *xdg_toplevel_new(struct wl_client *client, uint32_t version, uint32_t id,
                                             struct xdg_surface *xdg_surface) {
    struct xdg_toplevel *toplevel;

    toplevel = (struct xdg_toplevel *)malloc(sizeof(*toplevel));
    if (!toplevel) goto error0;
    toplevel->xdg_surface = xdg_surface;
    toplevel->resource = wl_resource_create(client, &xdg_toplevel_interface, version, id);
    if (!toplevel->resource) goto error1;
    // window_initialize(&toplevel->window, &toplevel_window_impl, xdg_surface->surface);
    toplevel->window = new Window(toplevel);
    rendererAddSurface(toplevel->window);
    wl_resource_set_implementation(toplevel->resource, &toplevel_impl, toplevel, &destroy_toplevel);
    // window_manage(&toplevel->window);

    return toplevel;

error1:
    free(toplevel);
error0:
    return NULL;
}

/* xdg_popup */
static void destroy_popup(struct wl_resource *resource) {
    struct xdg_popup *popup = static_cast<struct xdg_popup *>(wl_resource_get_user_data(resource));

    // compositor_view_destroy(popup->view);
    // std::cout << "destroy_popup" << std::endl;
    popup->popup->deleteLater();
    delete popup;
}

static void grab(struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat, uint32_t serial) {
    std::cout << "xdg_popup::grab" << std::endl;
}

static void reposition(struct wl_client *client, struct wl_resource *resource, struct wl_resource *positioner,
                       uint32_t token) {
    struct xdg_popup *popup = static_cast<struct xdg_popup *>(wl_resource_get_user_data(resource));
    struct xdg_positioner *pos = static_cast<struct xdg_positioner *>(wl_resource_get_user_data(resource));

    // std::cout << "x: " << pos->anchor_x << " " << pos->offset_x << std::endl;
    // std::cout << "y: " << pos->anchor_y << " " << pos->offset_y << std::endl;
    // popup->popup->setOffset(pos->anchor_x + pos->offset_x, pos->anchor_y + pos->offset_y);

    popup->xdg_surface->surface->surface_object->configure();

    xdg_popup_send_repositioned(resource, token);

    popup->xdg_surface->surface->surface_object->configure();
}

static const struct xdg_popup_interface popup_impl = {
    .destroy = destroy_resource,
    .grab = grab,
    .reposition = reposition,
};

static struct xdg_popup *xdg_popup_new(struct wl_client *client, uint32_t version, uint32_t id,
                                       struct xdg_surface *xdg_surface, struct xdg_surface *parent,
                                       struct xdg_positioner *positioner) {
    struct xdg_popup *popup;
    uint32_t serial = wl_display_next_serial(globals_wl.display);
    struct rect_3d rect;
    Window *window = nullptr;

    popup = new struct xdg_popup();
    if (!popup) goto error0;
    popup->xdg_surface = xdg_surface;
    popup->parent = parent;
    popup->positioner = *positioner;
    popup->resource = wl_resource_create(client, &xdg_popup_interface, version, id);
    if (!popup->resource) goto error1;
    popup->popup = new Popup(popup);
    popup->popup->setSize(positioner->width, positioner->height);
    popup->popup->setOffset(positioner->anchor_x + positioner->offset_x, positioner->anchor_y + positioner->offset_y);
    window = dynamic_cast<Window *>(parent->surface->surface_object);
    if (window != nullptr) {
        window->addPopup(popup->popup);
    }
    wl_resource_set_implementation(popup->resource, &popup_impl, popup, &destroy_popup);
    // popup->view = compositor_create_view(xdg_surface->surface);
    // std::cout << "new popup#" << id << std::endl;

    // rect = calculate_position(positioner);
    // TODO: set to reasonable value
    // compositor_view_set_parent(popup->view, parent_view);
    // view_move(&popup->view->base, parent_view->base.geometry.x + rect.x, parent_view->base.geometry.y + rect.y);
    // xdg_popup_send_configure(popup->resource, rect.x, rect.y, rect.width, rect.height);
    // xdg_surface_send_configure(xdg_surface->resource, serial);

    return popup;

error2:
    wl_resource_destroy(popup->resource);
error1:
    delete popup;
error0:
    return NULL;
}

/* xdg_surface */
static void get_toplevel(struct wl_client *client, struct wl_resource *resource, uint32_t id) {
    struct xdg_surface *xdg_surface = static_cast<struct xdg_surface *>(wl_resource_get_user_data(resource));
    struct xdg_toplevel *toplevel;

    if (xdg_surface->role) {
        wl_resource_post_error(resource, XDG_WM_BASE_ERROR_ROLE, "surface already has a role");
        return;
    }
    toplevel = xdg_toplevel_new(client, wl_resource_get_version(resource), id, xdg_surface);
    if (!toplevel) {
        wl_client_post_no_memory(client);
        return;
    }
    xdg_surface->role = toplevel->resource;
    wl_resource_add_destroy_listener(xdg_surface->role, &xdg_surface->role_destroy_listener);
}

static void get_popup(struct wl_client *client, struct wl_resource *resource, uint32_t id,
                      struct wl_resource *parent_resource, struct wl_resource *positioner_resource) {
    struct xdg_surface *xdg_surface = static_cast<struct xdg_surface *>(wl_resource_get_user_data(resource));
    struct xdg_surface *parent = static_cast<struct xdg_surface *>(wl_resource_get_user_data(parent_resource));
    struct xdg_positioner *positioner =
        static_cast<struct xdg_positioner *>(wl_resource_get_user_data(positioner_resource));
    struct xdg_popup *popup;

    if (xdg_surface->role) {
        wl_resource_post_error(resource, XDG_WM_BASE_ERROR_ROLE, "surface already has a role");
        return;
    }
    popup = xdg_popup_new(client, wl_resource_get_version(resource), id, xdg_surface, parent, positioner);
    if (!popup) {
        wl_client_post_no_memory(client);
        return;
    }
    xdg_surface->role = popup->resource;
    wl_resource_add_destroy_listener(xdg_surface->role, &xdg_surface->role_destroy_listener);
}

static void ack_configure(struct wl_client *client, struct wl_resource *resource, uint32_t serial) {
    struct xdg_surface *xdg_surface = static_cast<struct xdg_surface *>(wl_resource_get_user_data(resource));
    Window *window;

    if (!xdg_surface->role) return;
    // window = static_cast<Window *>(wl_resource_get_user_data(xdg_surface->role));
    // if (window && serial == xdg_surface->configure_serial) window->configure.acknowledged = true;
}

static void set_window_geometry(struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y,
                                int32_t width, int32_t height) {
    struct xdg_surface *xdg_surface = static_cast<struct xdg_surface *>(wl_resource_get_user_data(resource));
    xdg_surface->window_surface->setSize(width, height);
}

static const struct xdg_surface_interface xdg_surface_impl = {
    .destroy = destroy_resource,
    .get_toplevel = get_toplevel,
    .get_popup = get_popup,
    .set_window_geometry = set_window_geometry,
    .ack_configure = ack_configure,
};

static void handle_surface_destroy(struct wl_listener *listener, void *data) {
    struct xdg_surface *xdg_surface = wl_container_of(listener, xdg_surface, surface_destroy_listener);

    wl_resource_destroy(xdg_surface->resource);
}

static void handle_role_destroy(struct wl_listener *listener, void *data) {
    struct xdg_surface *xdg_surface = wl_container_of(listener, xdg_surface, role_destroy_listener);

    xdg_surface->role = NULL;
}

static void destroy_xdg_surface(struct wl_resource *resource) {
    struct xdg_surface *xdg_surface = static_cast<struct xdg_surface *>(wl_resource_get_user_data(resource));

    wl_list_remove(&xdg_surface->surface_destroy_listener.link);
    if (xdg_surface->role) wl_resource_destroy(xdg_surface->role);
    delete xdg_surface;
}

static struct xdg_surface *xdg_surface_new(struct wl_client *client, uint32_t version, uint32_t id,
                                           struct wl3d_surface *surface) {
    struct xdg_surface *xdg_surface;

    xdg_surface = new struct xdg_surface();
    if (!xdg_surface) goto error0;
    xdg_surface->resource = wl_resource_create(client, &xdg_surface_interface, version, id);
    if (!xdg_surface->resource) goto error1;
    xdg_surface->surface = surface;
    xdg_surface->surface_destroy_listener.notify = &handle_surface_destroy;
    xdg_surface->role = NULL;
    xdg_surface->role_destroy_listener.notify = &handle_role_destroy;
    wl_resource_add_destroy_listener(surface->resource, &xdg_surface->surface_destroy_listener);
    wl_resource_set_implementation(xdg_surface->resource, &xdg_surface_impl, xdg_surface, destroy_xdg_surface);

    return xdg_surface;

error1:
    delete xdg_surface;
error0:
    return NULL;
}

/* xdg_shell */
static void create_positioner(struct wl_client *client, struct wl_resource *resource, uint32_t id) {
    struct xdg_positioner *positioner;
    struct wl_resource *positioner_resource;
    uint32_t version;

    positioner = new xdg_positioner();
    if (!positioner) goto error0;

    version = wl_resource_get_version(resource);
    positioner_resource = wl_resource_create(client, &xdg_positioner_interface, version, id);
    if (!positioner_resource) goto error1;
    wl_resource_set_implementation(positioner_resource, &positioner_impl, positioner, &destroy_positioner);
    return;

error1:
    delete positioner;
error0:
    wl_resource_post_no_memory(resource);
}

static void get_xdg_surface(struct wl_client *client, struct wl_resource *resource, uint32_t id,
                            struct wl_resource *surface_resource) {
    struct xdg_surface *xdg_surface;
    struct wl3d_surface *surface = static_cast<struct wl3d_surface *>(wl_resource_get_user_data(surface_resource));

    xdg_surface = xdg_surface_new(client, wl_resource_get_version(resource), id, surface);
    if (!xdg_surface) wl_client_post_no_memory(client);
}

static void pong(struct wl_client *client, struct wl_resource *resource, uint32_t serial) {}

static const struct xdg_wm_base_interface wm_base_impl = {
    .destroy = destroy_resource,
    .create_positioner = create_positioner,
    .get_xdg_surface = get_xdg_surface,
    .pong = pong,
};

static void bind_wm_base(struct wl_client *client, void *data, uint32_t version, uint32_t id) {
    struct wl_resource *resource;

    resource = wl_resource_create(client, &xdg_wm_base_interface, version, id);
    if (!resource) {
        wl_client_post_no_memory(client);
        return;
    }
    wl_resource_set_implementation(resource, &wm_base_impl, NULL, NULL);
}

struct wl_global *xdg_shell_create(struct wl_display *display) {
    return wl_global_create(display, &xdg_wm_base_interface, 7, NULL, &bind_wm_base);
}
