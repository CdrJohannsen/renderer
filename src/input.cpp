#include "input.hpp"
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_timer.h>

#include <linux/input-event-codes.h>

#include <iostream>
#include "wayland/globals.h"
#include "wayland/keyboard.hpp"
#include "wayland/pointer.hpp"
#include "wayland/seat.h"
#include "wayland/surface.h"

#ifdef _DEBUG
#include "backends/imgui_impl_sdl3.h"
#endif

struct {
    bool buttonW = false;
    bool buttonA = false;
    bool buttonS = false;
    bool buttonD = false;
    bool buttonSHIFT = false;
    bool buttonSPACE = false;
    bool buttonCTRL = false;
} pressedButtons;

float cameraSpeed = 6.0f;

bool focused = false;
int32_t grabbedWindow = -1;
double grabbedDistance = INFINITY;
glm::vec3 grabbedRot;
bool resize = false;

bool handleInput(SDL_Event &e, FloatingCamera &camera, SDL_Window *window,
                 const std::vector<WindowSurface *> &windows) {
    while (SDL_PollEvent(&e)) {
#ifdef _DEBUG
        ImGui_ImplSDL3_ProcessEvent(&e);
#endif
        if (((e.type == SDL_EVENT_KEY_DOWN) || (e.type == SDL_EVENT_KEY_UP)) &&
            (globals_wl.seat->keyboard->focused != nullptr)) {
            globals_wl.seat->keyboard->key(e.key.timestamp, e.key.raw, e.type == SDL_EVENT_KEY_DOWN);
        }
        if (e.type == SDL_EVENT_QUIT)
            return false;
        else if (e.type == SDL_EVENT_KEY_DOWN) {
            if (e.key.key == SDLK_ESCAPE && !SDL_GetWindowRelativeMouseMode(window)) {
                SDL_SetWindowRelativeMouseMode(window, true);
            } else if (e.key.key == SDLK_ESCAPE && SDL_GetWindowRelativeMouseMode(window)) {
                SDL_SetWindowRelativeMouseMode(window, false);
            }
            if (SDL_GetWindowRelativeMouseMode(window) && (!focused)) {
                switch (e.key.key) {
                    case SDLK_W:
                        pressedButtons.buttonW = true;
                        break;
                    case SDLK_S:
                        pressedButtons.buttonS = true;
                        break;
                    case SDLK_A:
                        pressedButtons.buttonA = true;
                        break;
                    case SDLK_D:
                        pressedButtons.buttonD = true;
                        break;
                    case SDLK_SPACE:
                        pressedButtons.buttonSPACE = true;
                        break;
                    case SDLK_LSHIFT:
                        pressedButtons.buttonSHIFT = true;
                        break;
                    case SDLK_LCTRL:
                        pressedButtons.buttonCTRL = true;
                        break;
                }
            }
        } else if (e.type == SDL_EVENT_KEY_UP) {
            switch (e.key.key) {
                case SDLK_W:
                    pressedButtons.buttonW = false;
                    break;
                case SDLK_S:
                    pressedButtons.buttonS = false;
                    break;
                case SDLK_A:
                    pressedButtons.buttonA = false;
                    break;
                case SDLK_D:
                    pressedButtons.buttonD = false;
                    break;
                case SDLK_SPACE:
                    pressedButtons.buttonSPACE = false;
                    break;
                case SDLK_LSHIFT:
                    pressedButtons.buttonSHIFT = false;
                    break;
                case SDLK_LCTRL:
                    pressedButtons.buttonCTRL = false;
                    break;
            }
            if (e.key.key == SDLK_LALT) {
                grabbedWindow = -1;
                resize = false;
            }
        } else if (e.type == SDL_EVENT_MOUSE_MOTION) {
            if (SDL_GetWindowRelativeMouseMode(window)) {
                if (grabbedWindow == -1) {
                    double minDistance = INFINITY;
                    int32_t closestIndex = -1;
                    for (size_t i = 0; i < windows.size(); i++) {
                        double distance = windows[i]->getDistance(camera.getPosition(), camera.getLookAt());
                        if (distance < minDistance) {
                            closestIndex = i;
                            minDistance = distance;
                        }
                    }
                    if (closestIndex >= 0) {
                        windows[closestIndex]->motion(camera.getPosition(), camera.getLookAt());
                    }
                }
                glm::vec3 oldRot = camera.getLookAt();
                camera.onMouseMove(e.motion.xrel, e.motion.yrel);
                if (grabbedWindow != -1) {
                    glm::vec3 rot = camera.getLookAt();
                    if (!resize) {
                        windows[grabbedWindow]->rotate(camera.getPosition(), rot);
                        windows[grabbedWindow]->move((rot * glm::vec3(grabbedDistance)) -
                                                     (oldRot * glm::vec3(grabbedDistance)));
                    } else {
                        glm::vec3 diff = (rot - oldRot) * static_cast<float>(grabbedDistance);
                        glm::vec2 res = {glm::dot(glm::cross(rot, {0, 1, 0}), diff), diff.y};
                        windows[grabbedWindow]->resize(res);
                    }
                }
            }
        } else if ((SDL_GetModState() & SDL_KMOD_ALT) && (e.button.button == SDL_BUTTON_MIDDLE) &&
                   (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) && focused) {
            focused = false;
            if (globals_wl.seat->keyboard->focused &&
                dynamic_cast<WindowSurface *>(globals_wl.seat->keyboard->focused->surface_object)) {
                dynamic_cast<WindowSurface *>(globals_wl.seat->keyboard->focused->surface_object)->leave();
            }
        } else if (((e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) || (e.type == SDL_EVENT_MOUSE_BUTTON_UP)) &&
                   (globals_wl.seat->pointer->focused != nullptr)) {
            if (e.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                grabbedWindow = -1;
                resize = false;
            }
            if (SDL_GetModState() & SDL_KMOD_ALT) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                        double minDistance = INFINITY;
                        int32_t closestIndex = -1;
                        for (size_t i = 0; i < windows.size(); i++) {
                            double distance = windows[i]->getDistance(camera.getPosition(), camera.getLookAt());
                            if (distance < minDistance) {
                                closestIndex = i;
                                minDistance = distance;
                            }
                        }
                        grabbedWindow = closestIndex;
                        grabbedDistance = minDistance;
                    }
                } else if (e.button.button == SDL_BUTTON_MIDDLE) {
                    if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                        focused = true;
                        double minDistance = INFINITY;
                        int32_t closestIndex = -1;
                        for (size_t i = 0; i < windows.size(); i++) {
                            double distance = windows[i]->getDistance(camera.getPosition(), camera.getLookAt());
                            if (distance < minDistance) {
                                closestIndex = i;
                                minDistance = distance;
                            }
                        }
                        if (closestIndex >= 0) {
                            windows[closestIndex]->enter(camera.getPosition(), camera.getLookAt());
                        }
                    }
                } else if (e.button.button == SDL_BUTTON_RIGHT) {
                    if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                        double minDistance = INFINITY;
                        int32_t closestIndex = -1;
                        for (size_t i = 0; i < windows.size(); i++) {
                            double distance = windows[i]->getDistance(camera.getPosition(), camera.getLookAt());
                            if (distance < minDistance) {
                                closestIndex = i;
                                minDistance = distance;
                            }
                        }
                        grabbedWindow = closestIndex;
                        grabbedDistance = minDistance;
                        resize = true;
                        grabbedRot = camera.getLookAt();
                    }
                }
            } else {
                uint32_t button = 0;
                switch (e.button.button) {
                    case SDL_BUTTON_LEFT:
                        button = BTN_LEFT;
                        break;
                    case SDL_BUTTON_RIGHT:
                        button = BTN_RIGHT;
                        break;
                    case SDL_BUTTON_MIDDLE:
                        button = BTN_MIDDLE;
                        break;
                    default:
                        cout << "Unknown mouse button: " << e.button.button << endl;
                }
                globals_wl.seat->pointer->button(e.button.timestamp, button, e.type == SDL_EVENT_MOUSE_BUTTON_DOWN);
            }
        } else if (e.type == SDL_EVENT_MOUSE_WHEEL) {
            globals_wl.seat->pointer->axis(e.wheel.timestamp, e.wheel.x * 15, e.wheel.y * -15);
        }
    }
    return true;
}

void handleMovement(FloatingCamera &camera, float &delta, const std::vector<WindowSurface *> &windows) {
    glm::vec3 oldPos = camera.getPosition();
    if (pressedButtons.buttonW) {
        camera.moveFront(delta * cameraSpeed);
    }
    if (pressedButtons.buttonS) {
        camera.moveFront(delta * -cameraSpeed);
    }
    if (pressedButtons.buttonA) {
        camera.moveSide(delta * -cameraSpeed);
    }
    if (pressedButtons.buttonD) {
        camera.moveSide(delta * cameraSpeed);
    }
    if (pressedButtons.buttonSPACE) {
        camera.moveUp(delta * cameraSpeed);
    }
    if (pressedButtons.buttonSHIFT) {
        camera.moveUp(delta * -cameraSpeed);
    }
    if (pressedButtons.buttonCTRL) {
        cameraSpeed = 32.0f;
    }
    if (!pressedButtons.buttonCTRL && (cameraSpeed == 32.0f)) {
        cameraSpeed = 6.0f;
    }

    glm::vec3 pos = camera.getPosition();
    if (grabbedWindow != -1) {
        windows[grabbedWindow]->move(pos - oldPos);
    }
}
