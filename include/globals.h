#pragma once

#include "layer.hpp"
#include "window_surface.hpp"

void rendererAddSurface(WindowSurface *surface);
void rendererRemoveSurface(WindowSurface *window);

void rendererAddSkybox(Layer *layer);
void rendererRemoveSkybox(Layer *layer);

void rendererAddOverlay(Layer *layer);
void rendererRemoveOverlay(Layer *layer);
