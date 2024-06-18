#pragma once
#include <SDL2/SDL.h>

#include "floating_camera.hpp"

bool handleInput(SDL_Event &e, FloatingCamera &camera);

void handleMovement(FloatingCamera &camera, float &delta);
