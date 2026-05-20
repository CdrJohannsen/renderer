#pragma once

#include <SDL3/SDL.h>
#include "floating_camera.hpp"
#include "framebuffer.hpp"
#include "gbuffer.hpp"
#include "layer.hpp"
#include "object.hpp"
#include "shader.hpp"
#include "skybox.hpp"
#include "window_surface.hpp"

#include <algorithm>

#ifdef _DEBUG
#include "imgui.h"
#endif

#define WIDTH 1920.0f
#define HEIGHT 1080.0f

class Renderer {
   public:
    Renderer(vector<string> args);
    ~Renderer();
    bool frame(float delta);
    void addWindow(WindowSurface *window) {
        window->init(windowShader);
        windows.push_back(window);
    }
    void removeWindow(WindowSurface *window) {
        windows.erase(std::remove(windows.begin(), windows.end(), window), windows.end());
    }

    void addSkybox(Layer *layer) {
        layer->init(skyboxShader);
        skyboxes.push_back(layer);
    }
    void removeSkybox(Layer *layer) {
        skyboxes.erase(std::remove(skyboxes.begin(), skyboxes.end(), layer), skyboxes.end());
    }

    void addOverlay(Layer *layer) {
        layer->init(overlayShader);
        overlays.push_back(layer);
    }
    void removeOverlay(Layer *layer) {
        overlays.erase(std::remove(overlays.begin(), overlays.end(), layer), overlays.end());
    }

   private:
    SDL_Event e;
    SDL_GLContext glContext;
    SDL_Window *window;
    GBuffer gBuffer;
    FrameBuffer framebuffer;
    FloatingCamera camera = FloatingCamera(90.0f, WIDTH, HEIGHT);

#ifdef _DEBUG
    ImGuiIO *io;
#endif

    std::vector<Object *> objects;
    std::vector<WindowSurface *> windows{};
    std::vector<Layer *> skyboxes{};
    std::vector<Layer *> overlays{};

    Shader *fontShader;
    Shader *postProcessShader;
    Shader *gBufferShader;
    Shader *windowShader;
    Shader *deferredShader;
    Shader *skyboxShader;
    Shader *overlayShader;

    Skybox skybox_;
    GLuint skyboxTexture;
};
