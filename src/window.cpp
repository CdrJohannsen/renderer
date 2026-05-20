
#include "window.hpp"
#include <SDL3/SDL_timer.h>
#include <wayland-server-core.h>
#include <wayland-server-protocol.h>
#include <wayland-util.h>
#include "wayland/globals.h"
#include "wayland/keyboard.hpp"
#include "wayland/output.hpp"
#include "wayland/pointer.hpp"
#include "wayland/seat.h"
#include "wayland/surface.h"
#include "wayland/xdg-shell.h"

#include <csignal>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#include "imgui.h"
#include "xdg-shell-protocol.h"

Window::Window(struct xdg_toplevel *toplevel) : WindowSurface(toplevel->xdg_surface) {
    toplevel_ = toplevel;

    updateModelMat();

    uint32_t capabilities[] = {XDG_TOPLEVEL_WM_CAPABILITIES_MINIMIZE};
    struct wl_array wl_capabilities = {.size = sizeof(capabilities), .data = capabilities};
    xdg_toplevel_send_wm_capabilities(toplevel_->resource, &wl_capabilities);

    configure();
}

void Window::close() {
    xdg_toplevel_send_close(toplevel_->resource);
    if (focused) {
        focused = false;
        globals_wl.seat->keyboard->leave(surface_);
        globals_wl.seat->pointer->leave(surface_);
    }
}

void Window::configure() {
    uint32_t states[] = {XDG_TOPLEVEL_STATE_ACTIVATED};
    struct wl_array wl_states = {.size = sizeof(states), .data = states};
    xdg_toplevel_send_configure(toplevel_->resource, size.x, size.y, &wl_states);
    WindowSurface::configure();
}

void Window::render(FloatingCamera *camera) {
    for (int32_t i = popups_.size() - 1; i >= 0; i--) {
        if (popups_[i]->toDelete()) {
            Popup *popup = popups_[i];
            popups_.erase(std::remove(popups_.begin(), popups_.end(), popup), popups_.end());
            delete popup;
        }
    }
    for (Popup *const popup : popups_) {
        if (popup == nullptr) {
            raise(SIGINT);
        }
        popup->render(camera);
    }
    WindowSurface::render(camera);
}

void Window::motion(glm::vec3 pos, glm::vec3 rot) {
    double minDistance = WindowSurface::getDistance(pos, rot);
    int32_t closestIndex = -1;
    for (size_t i = 0; i < popups_.size(); i++) {
        double distance = popups_[i]->getDistance(pos, rot);
        if (distance < minDistance) {
            closestIndex = i;
            minDistance = distance;
        }
    }
    if (closestIndex >= 0) {
        popups_[closestIndex]->motion(pos, rot);
    } else {
        WindowSurface::motion(pos, rot);
    }
}

double Window::getDistance(glm::vec3 pos, glm::vec3 rot) {
    if (delete_later || (!surface_->state.buffer) || (!surface_->state.buffer_resource)) return INFINITY;
    double minDistance = WindowSurface::getDistance(pos, rot);
    for (Popup *popup : popups_) {
        double distance = popup->getDistance(pos, rot);
        if (distance < minDistance) {
            minDistance = distance;
        }
    }
    return minDistance;
}

void Window::updateModelMat() {
    modelMat = glm::mat4(1);
    modelMat = glm::translate(modelMat, position);
    // glm::vec3 rotOffset = rotation * (size.x / kSurfaceScaling);
    // modelMat = glm::translate(modelMat, glm::vec3((size.x / kSurfaceScaling) - (rotOffset.x), 0, -rotOffset.z));
    modelMat = glm::rotate(modelMat, asin(rotation.y), glm::normalize(glm::cross(rotation, {0, 1, 0})));
    modelMat = glm::rotate(modelMat, atan2(rotation.x, rotation.z), {0, 1, 0});

    for (Popup *const popup : popups_) {
        popup->setParentSize(size);
        popup->setParentMat(modelMat);
    }
}

#ifdef _DEBUG
void Window::renderDebugUI() {
    ImGui::PushID(this);
    if (ImGui::TreeNode(title_.c_str())) {
        // glm::vec3 rot = {glm::degrees(asin(rotation.x)), glm::degrees(asin(rotation.y)),
        //                  glm::degrees(asin(rotation.z))};
        glm::vec2 s = size;
        ImGui::DragFloat2("Size", (float *)&s, 1, -FLT_MAX, +FLT_MAX);
        ImGui::DragFloat3("Pos", (float *)&position, 0.1, -FLT_MAX, +FLT_MAX);
        // ImGui::DragFloat3("Rot", (float *)&rot, 1, -360, 360);
        // rotation = {sin(glm::radians(rot.x)), sin(glm::radians(rot.y)), sin(glm::radians(rot.z))};
        if (s != size) {
            size = s;
            configure();
        }
        if (ImGui::Button("X")) {
            close();
        }
        if (!popups_.empty() && ImGui::TreeNode("Popups")) {
            for (Popup *const popup : popups_) {
                popup->renderDebugUI();
            }
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
    ImGui::PopID();
}
#endif
