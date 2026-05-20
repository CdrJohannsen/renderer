#pragma once
#include <SDL3/SDL.h>
#include <vector>

#include "floating_camera.hpp"
#include "window_surface.hpp"

bool handleInput(SDL_Event &e, FloatingCamera &camera, SDL_Window *window, const std::vector<WindowSurface *> &windows);

void handleMovement(FloatingCamera &camera, float &delta, const std::vector<WindowSurface *> &windows);
