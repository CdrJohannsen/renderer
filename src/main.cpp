
#include <SDL3/SDL_error.h>
#include <stdlib.h>

#include <iostream>
using namespace std;
#include <SDL3/SDL.h>
#define GLEW_STATIC
#include <GL/glew.h>
#define SDL_MAIN_HANDLED
#define STB_IMAGE_IMPLEMENTATION
// #include <GL/gl.h>
// #include <SDL3/SDL_opengl.h>
#include <stb/stb_image.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <wayland-server.h>

#include "renderer.h"
#include "wayland/wayland.h"

#define WIDTH 1920.0f
#define HEIGHT 1080.0f
#define RATIO (WIDTH / HEIGHT)

Renderer *renderer = nullptr;

int main(int argc, char **argv) {
    vector<string> args;
    if (argc <= 1) {
        args.push_back("./ressources/box.mod");
    } else {
        args.assign(argv + 1, argv + argc);
    }
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

#ifdef _DEBUG
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    renderer = new Renderer(args);

#ifdef WL3D
    /* Wayland */
    struct wl_display *display = wl_display_create();
    if (!display) {
        fprintf(stderr, "Unable to create Wayland display.\n");
        return 1;
    }

    const int socket = wl_display_add_socket(display, "wayland-5");
    if (socket != 0) {
        fprintf(stderr, "Unable to add socket to Wayland display.\n");
        return 1;
    }

    struct wl_event_loop *event_loop = wl_display_get_event_loop(display);

    init_wayland(display);
#endif

    uint64_t perfCounterFrequency = SDL_GetPerformanceFrequency();
    uint64_t lastCounter = SDL_GetPerformanceCounter();
    float delta = 0.0f;
    uint64_t endCounter = SDL_GetPerformanceCounter();
    uint64_t counterElapsed = endCounter - lastCounter;
    bool running = true;
    while (running) {
#ifdef WL3D
        wl_display_flush_clients(display);
        wl_event_loop_dispatch(event_loop, 1);
#endif

        running = renderer->frame(delta);

        endCounter = SDL_GetPerformanceCounter();
        counterElapsed = endCounter - lastCounter;
        delta = ((float)counterElapsed) / ((float)perfCounterFrequency);
        lastCounter = endCounter;
    }

#ifdef WL3D
    wl_display_destroy(display);
#endif

    return 0;
}
