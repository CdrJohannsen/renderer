#include "globals.h"
#include "renderer.h"

extern Renderer *renderer;

void rendererAddSurface(WindowSurface *window) { renderer->addWindow(window); }
void rendererRemoveSurface(WindowSurface *window) { renderer->removeWindow(window); }

void rendererAddSkybox(Layer *layer) { renderer->addSkybox(layer); }
void rendererRemoveSkybox(Layer *layer) { renderer->removeSkybox(layer); }

void rendererAddOverlay(Layer *layer) { renderer->addOverlay(layer); }
void rendererRemoveOverlay(Layer *layer) { renderer->removeOverlay(layer); }
