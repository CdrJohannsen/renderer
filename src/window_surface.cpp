
#include "window_surface.hpp"
#include <SDL3/SDL_timer.h>
#include <wayland-server-core.h>
#include <wayland-server-protocol.h>
#include <wayland-util.h>
#include "globals.h"
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
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#include "imgui.h"
#include "xdg-shell-protocol.h"

WindowSurface::WindowSurface(struct xdg_surface *surface) : Surface(surface->surface) {
    xdg_surface_ = surface;
    surface_->surface_object = this;
    xdg_surface_->window_surface = this;
    position.z = 3;
}

WindowSurface::~WindowSurface() {
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &IBO);
    if (focused) {
        focused = false;
        globals_wl.seat->keyboard->leave(surface_);
        globals_wl.seat->pointer->leave(surface_);
    }
}
void WindowSurface::configure() {
    uint32_t serial = wl_display_next_serial(globals_wl.display);
    xdg_surface_send_configure(xdg_surface_->resource, serial);
    xdg_surface_->configure_serial = serial;
}

void WindowSurface::init(Shader *windowShader) {
    windowShader_ = windowShader;
    windowShader->bind();
    // Model.init
    glGenTextures(1, &window_texture);
    glBindTexture(GL_TEXTURE_2D, window_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);

    // setup plane VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    // clang-format off
    float verts[4*8] = {
        1,0,0,0,0,1,1,1,
        0,1,0,0,0,1,0,0,
        1,1,0,0,0,1,1,0,
        0,0,0,0,0,1,0,1,
    };
    // clang-format on
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), &verts, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));

    glBindVertexArray(0);

    // IndexBuffer
    uint32_t indices[6] = {0, 2, 1, 0, 1, 3};
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(indices[0]), indices, GL_DYNAMIC_DRAW);

    // Mesh
    colorLocation = glGetUniformLocation(windowShader->getShaderID(), "u_window_texture");

    // Object
    modelMat = glm::mat4(1.0f);
    modelMat = glm::scale(modelMat, glm::vec3(1.0f));
    modelViewLocation = glGetUniformLocation(windowShader->getShaderID(), "u_modelView");
    modelViewProjLocation = glGetUniformLocation(windowShader->getShaderID(), "u_modelViewProj");
}

void WindowSurface::render(FloatingCamera *camera) {
    if (delete_later || (!surface_->state.buffer) || (!surface_->state.buffer_resource)) return;
    updateModelMat();
    glDisable(GL_CULL_FACE);

    float scaled_width = surface_->state.buffer->width / kSurfaceScaling;
    float scaled_height = surface_->state.buffer->height / kSurfaceScaling;
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float), &scaled_width);
    glBufferSubData(GL_ARRAY_BUFFER, 16 * sizeof(float), sizeof(float), &scaled_width);
    glBufferSubData(GL_ARRAY_BUFFER, 9 * sizeof(float), sizeof(float), &scaled_height);
    glBufferSubData(GL_ARRAY_BUFFER, 17 * sizeof(float), sizeof(float), &scaled_height);

    // Object.render
    modelViewProj = camera->getViewProj() * modelMat;
    modelView = camera->getView() * modelMat;
    glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, &modelView[0][0]);
    glUniformMatrix4fv(modelViewProjLocation, 1, GL_FALSE, &modelViewProj[0][0]);

    // IndexBuffer.bind
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    // Mesh.render
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, window_texture);
    glUniform1i(colorLocation, 0);

    glActiveTexture(GL_TEXTURE0);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glEnable(GL_CULL_FACE);

    struct wl_resource *resource, *tmp;

    wl_list_for_each_safe(resource, tmp, &surface_->state.frame_callbacks, link) {
        wl_callback_send_done(resource, SDL_GetTicks());
        wl_resource_destroy(resource);
    }
}

void WindowSurface::commit() {
    if (delete_later || (!surface_->state.buffer) || (!surface_->state.buffer_resource)) return;

    glBindTexture(GL_TEXTURE_2D, window_texture);
    if ((surface_->state.buffer->height > 0) && (surface_->state.buffer->width > 0))
        glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA, surface_->state.buffer->width, surface_->state.buffer->height, 0,
                     GL_BGRA, GL_UNSIGNED_BYTE, surface_->state.buffer->data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void WindowSurface::enter(glm::vec3 pos, glm::vec3 rot) {
    globals_wl.seat->keyboard->enter(surface_);
    focused = true;
}

void WindowSurface::leave() {
    globals_wl.seat->keyboard->leave(surface_);
    focused = false;
}

void WindowSurface::motion(glm::vec3 pos, glm::vec3 rot) {
    glm::vec3 scale{};
    glm::quat orientation{};
    glm::vec3 translation{};
    glm::vec3 skew{};
    glm::vec4 perspective{};
    glm::decompose(modelMat, scale, orientation, translation, skew, perspective);

    glm::vec3 normal = glm::mat3_cast(orientation) * glm::vec3(0, 0, 1);
    glm::vec3 diff = pos - translation;
    glm::vec3 p = (((diff + translation) + ((-glm::dot(diff, normal) / glm::dot(rot, normal)) * rot)) - translation) *
                  glm::mat3_cast(-orientation);

    double x, y;
    x = p.x * kSurfaceScaling;
    y = -p.y * kSurfaceScaling + surface_->state.buffer->height;
    if (((x > 0) && (x < surface_->state.buffer->width)) && ((y > 0) && (y < surface_->state.buffer->height))) {
        if (globals_wl.seat->pointer->focused != surface_) {
            globals_wl.seat->pointer->enter(surface_);
            configure();
        }
    } else if (globals_wl.seat->pointer->focused == surface_) {
        globals_wl.seat->pointer->leave(surface_);
        configure();
    }
    globals_wl.seat->pointer->motion(SDL_GetTicks(), x, y);
}

double WindowSurface::getDistance(glm::vec3 pos, glm::vec3 rot) {
    if (delete_later || (!surface_->state.buffer) || (!surface_->state.buffer_resource)) return INFINITY;
    glm::vec3 scale{};
    glm::quat orientation{};
    glm::vec3 translation{};
    glm::vec3 skew{};
    glm::vec4 perspective{};
    glm::decompose(modelMat, scale, orientation, translation, skew, perspective);
    glm::vec3 normal = glm::mat3_cast(orientation) * glm::vec3(0, 0, 1);
    if (glm::dot(rot, normal) < 0) {
        glm::vec3 diff = pos - translation;
        glm::vec3 p =
            (((diff + translation) + ((-glm::dot(diff, normal) / glm::dot(rot, normal)) * rot)) - translation);
        // glm::vec3 rel = p * glm::mat3_cast(-orientation);
        glm::vec3 rel = p * -orientation;
        p += translation;
        double x, y;
        x = rel.x * kSurfaceScaling;
        y = -rel.y * kSurfaceScaling + surface_->state.buffer->height;
        if (((x > 0) && (x < surface_->state.buffer->width)) && ((y > 0) && (y < surface_->state.buffer->height))) {
            return abs(glm::length(pos - p));
        } else {
            return INFINITY;
        }
    } else {
        return INFINITY;
    }
}

void WindowSurface::rotate(glm::vec3 pos, glm::vec3 rot) {
    glm::vec3 scale{};
    glm::quat orientation{};
    glm::vec3 translation{};
    glm::vec3 skew{};
    glm::vec4 perspective{};
    glm::decompose(modelMat, scale, orientation, translation, skew, perspective);
    glm::vec3 normal = glm::mat3_cast(orientation) * glm::vec3(0., 0., 1.);
    glm::vec3 diff = pos - translation;
    glm::vec3 p = (((diff + translation) + ((-glm::dot(diff, normal) / glm::dot(rot, normal)) * rot)) - translation) *
                  glm::mat3_cast(-orientation);

    // TODO
    // std::cout << p.x << " \t" << p.y << " \t" << p.z << std::endl;
    glm::vec3 oldRot = -glm::normalize(glm::cross(rotation, {0., 1., 0.}));
    rotation = glm::normalize(-rot);
    glm::vec3 newRot = -glm::normalize(glm::cross(rotation, {0., 1., 0.}));

    position.x += (oldRot.x * p.x);
    position.z += (oldRot.z * p.x);
    position.x -= (newRot.x * p.x);
    position.z -= (newRot.z * p.x);
}

void WindowSurface::move(glm::vec3 mov) { position += mov; }

void WindowSurface::resize(glm::vec2 diff) {
    size.x += diff.x * kSurfaceScaling;
    size.y += diff.y * kSurfaceScaling;
    if ((maxSize_.x > 0) && (maxSize_.y > 0)) {
        size = min(size, maxSize_);
    }
    size = max(size, minSize_);
    configure();
}

void WindowSurface::setPosition(glm::vec3 pos) { position = pos; }

#ifdef _DEBUG

void WindowSurface::renderDebugUI() {
    ImGui::PushID(this);
    if (ImGui::TreeNode(title_.c_str())) {
        ImGui::Text("Size %f %f", size.x, size.y);
        glm::vec3 rot = {glm::degrees(asin(rotation.x)), glm::degrees(asin(rotation.y)),
                         glm::degrees(asin(rotation.z))};
        ImGui::DragFloat3("Pos", (float *)&position, 0.1, FLT_MIN, FLT_MAX);
        ImGui::DragFloat3("Rot", (float *)&rot, 1, -360, 360);
        rotation = {sin(glm::radians(rot.x)), sin(glm::radians(rot.y)), sin(glm::radians(rot.z))};
        if (ImGui::Button("X")) {
            close();
        }
        ImGui::TreePop();
    }
    ImGui::PopID();
}

#endif
