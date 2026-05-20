
#include "popup.hpp"
#include <SDL3/SDL_timer.h>
#include <wayland-server-core.h>
#include <wayland-server-protocol.h>
#include <wayland-util.h>
#include "globals.h"
#include "imgui.h"
#include "wayland/globals.h"
#include "wayland/keyboard.hpp"
#include "wayland/output.hpp"
#include "wayland/pointer.hpp"
#include "wayland/seat.h"
#include "wayland/surface.h"
#include "wayland/xdg-shell.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>

Popup::Popup(struct xdg_popup *popup) : WindowSurface(popup->xdg_surface) {
    popup_ = popup;

    title_ = "popup#" + std::to_string(wl_resource_get_id(popup_->resource));

    updateModelMat();

    configure();
}

void Popup::close() {
    xdg_popup_send_popup_done(popup_->resource);
    if (focused) {
        focused = false;
        globals_wl.seat->keyboard->leave(surface_);
        globals_wl.seat->pointer->leave(surface_);
    }
    rendererRemoveSurface(this);
}

void Popup::configure() {
    xdg_popup_send_configure(popup_->resource, surface_->pending.x, surface_->pending.y, size.x, size.y);
    WindowSurface::configure();
    updateModelMat();
}

void Popup::updateModelMat() {
    modelMat = glm::translate(parentMat_,
                              glm::vec3(offset_.x, parentSize_.y - offset_.y - (size.y / kSurfaceScaling), offset_.z));
}
