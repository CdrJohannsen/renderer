#include <csignal>
#include <iostream>
#include "wayland/surface.h"
#include "window_surface.hpp"
using namespace std;

#define GLEW_STATIC
#include <GL/glew.h>
#define SDL_MAIN_HANDLED
#ifdef _DEBUG
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl3.h"
#include "imgui.h"
#endif
#include <stb/stb_image.h>

#include "floating_camera.hpp"
#include "font.hpp"
#include "framebuffer.hpp"
#include "gbuffer.hpp"
#include "input.hpp"
#include "object.hpp"
#include "renderUtils.hpp"
#include "shader.hpp"

#include "renderer.h"

#define WIDTH 1920.0f
#define HEIGHT 1080.0f
#define RATIO (WIDTH / HEIGHT)

static void OpenGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                const GLchar *message, const void *userParam) {
    cout << "[OpenGL Error] (";
    switch (source) {
        case GL_DEBUG_SOURCE_API:
            std::cout << "API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            std::cout << "Window System";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            std::cout << "Shader Compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            std::cout << "Third Party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            std::cout << "Application";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            std::cout << "Other";
            break;
    }
    std::cout << ") " << message << std::endl;
}

Renderer::Renderer(vector<string> args) {
    uint32_t flags = SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN;
    window = SDL_CreateWindow("Renderer", WIDTH, HEIGHT, flags);
    glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        cout << "No context" << endl;
    }

    GLenum err = glewInit();
    if (err != GLEW_OK && err != 4) {
        cout << "Error: " << glewGetErrorString(err) << endl;
        cout << err << endl;
        // cin.get();
        return;
    }

    cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // Vsync = -1
    SDL_GL_SetSwapInterval(0);

    SDL_SetWindowRelativeMouseMode(window, true);

#ifdef _DEBUG
    // IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO();
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForOpenGL(window, glContext);
    const char *glsl_version = "#version 120";
    ImGui_ImplOpenGL3_Init(glsl_version);
#endif

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    /*glEnable(GL_MULTISAMPLE);*/

#ifdef _DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(OpenGLDebugCallback, 0);
#endif

    Font font;
    font.initFont((char *)"ressources/fonts/liberation-sans/LiberationSans-Bold.ttf");

    fontShader = new Shader("shaders/font.vert", "shaders/font.frag");
    postProcessShader = new Shader("shaders/post.vert", "shaders/post.frag");
    skyboxShader = new Shader("shaders/sky.vert", "shaders/sky.frag");
    gBufferShader = new Shader("shaders/gBuffer.vert", "shaders/gBuffer.frag");
    windowShader = new Shader("shaders/window.vert", "shaders/window.frag");
    deferredShader = new Shader("shaders/deferred.vert", "shaders/deferred.frag");
    overlayShader = new Shader("shaders/overlay.vert", "shaders/overlay.frag");

    glGenTextures(1, &skyboxTexture);
    glBindTexture(GL_TEXTURE_2D, skyboxTexture);

    int32_t textureWidth = 0;
    int32_t textureHeigth = 0;
    int32_t bitsPerPixel = 0;
    stbi_set_flip_vertically_on_load(false);
    auto textureBuffer =
        stbi_load("ressources/rogland_clear_night_4k.png", &textureWidth, &textureHeigth, &bitsPerPixel, 4);
    stbi_set_flip_vertically_on_load(true);
    if (textureBuffer) {
        glBindTexture(GL_TEXTURE_2D, skyboxTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureWidth, textureHeigth, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     textureBuffer);

        stbi_image_free(textureBuffer);
    } else {
        cerr << "Failed to load skybox " << stbi_failure_reason() << endl;
    }
    skybox_.init();

    // Model modelTree;
    // modelTree.init(argv[1],&shader);

    for (string arg : args) {
        objects.push_back(new Object(arg, gBufferShader, deferredShader, {0, 0, 0}));
    }

    // camera.translate(glm::vec3(0.0f,0.0f,5.0f));
    camera.update();

    // int textureUniformLocation =
    // glGetUniformLocation(shader.getShaderID(),"u_texture"); if
    // (textureUniformLocation != -1){ glUniform1i(textureUniformLocation,0);
    // }
    int w, h;
    // SDL_GetWindowSize(window, &w, &h);
    w = WIDTH;
    h = HEIGHT;

    framebuffer.create(w, h);

    gBuffer.create(w, h);
    deferredShader->bind();
    deferredShader->setInt("gPosition", 0);
    deferredShader->setInt("gNormal", 1);
    deferredShader->setInt("gColor", 2);
    deferredShader->setInt("gEmissive", 3);
    deferredShader->setInt("gMRA", 4);

    /* Main loop preparation */
    glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
}

Renderer::~Renderer() {
#ifdef _DEBUG
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
#endif
    SDL_GL_DestroyContext(glContext);
    SDL_DestroyWindow(window);
    framebuffer.destroy();
    SDL_SetWindowRelativeMouseMode(window, false);
}

/**
 * @brief Draw the frame
 */
bool Renderer::frame(float delta) {
    for (int32_t i = windows.size() - 1; i >= 0; i--) {
        if (windows[i]->toDelete()) {
            WindowSurface *window = windows[i];
            removeWindow(window);
            delete window;
        }
    }

    bool running = handleInput(e, camera, window, windows);
    handleMovement(camera, delta, windows);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera.update();

#ifdef _DEBUG
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
#endif

    // glActiveTexture(GL_TEXTURE0);

    // modelTree.render(camera.getView(),glm::mat4(1.0f));
    gBuffer.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gBufferShader->bind();

    for (Object *object : objects) {
        object->render(&camera);
    }
    gBufferShader->unbind();

    windowShader->bind();
    for (WindowSurface *window : windows) {
        if (window == nullptr) {
            raise(SIGINT);
        }
        window->render(&camera);
    }
    windowShader->unbind();

    gBuffer.unbind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    framebuffer.bind();
    deferredShader->bind();
    gBuffer.bindTexture();
    for (Object *object : objects) {
        object->updateLights(camera);
    }
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderQuad();
    deferredShader->unbind();
    // gBuffer.blitFramebuffer();

    // Skybox

    glm::mat4 projection = camera.getProj();
    glm::mat4 view = glm::mat4(glm::mat3(camera.getView()));
    skyboxShader->bind();
    skyboxShader->setMat4("u_view", view);
    skyboxShader->setMat4("u_projection", projection);
    if (!skyboxes.empty()) {
        for (int32_t i = skyboxes.size() - 1; i >= 0; i--) {
            skyboxes[i]->render(&camera);
            if (skyboxes[i]->toDelete()) {
                Layer *skybox = skyboxes[i];
                removeSkybox(skybox);
                delete skybox;
            }
        }
    } else {
        skybox_.render(skyboxTexture);
    }
    skyboxShader->unbind();

    // glDepthFunc(GL_LEQUAL);
    // glm::mat4 projection = camera.getProj();
    // glm::mat4 view = glm::mat4(glm::mat3(camera.getView()));
    // skyboxShader->bind();
    // skyboxShader->setMat4("u_view", view);
    // skyboxShader->setMat4("u_projection", projection);
    // glBindVertexArray(skyboxVAO);
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, cubemapTexture);
    // glDrawArrays(GL_TRIANGLES, 0, 36);
    // skyboxShader->unbind();
    // glDepthFunc(GL_LESS);

    // Overlay

    overlayShader->bind();
        for (int32_t i = overlays.size() - 1; i >= 0; i--) {
            overlays[i]->render(&camera);
            if (overlays[i]->toDelete()) {
                Layer *overlay = overlays[i];
                removeOverlay(overlay);
                delete overlay;
            }
        }
    overlayShader->unbind();

    framebuffer.unbind();

    // Post Processing

    postProcessShader->bind();
    GLuint pPTextureLocation = glGetUniformLocation(postProcessShader->getShaderID(), "u_texture");
    GLuint pPDepthLocation = glGetUniformLocation(postProcessShader->getShaderID(), "u_depth");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, framebuffer.getTextureID(0));
    glUniform1i(pPTextureLocation, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, framebuffer.getTextureID(1));
    glUniform1i(pPDepthLocation, 1);

    glActiveTexture(GL_TEXTURE0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    postProcessShader->unbind();
    // Font overlay
    /*
    fontShader.bind();

    int w,h;
    SDL_GetWindowSize(window, &w, &h);
    // cout << w << " " << h << endl;
    glm::mat4 ortho = glm::ortho(0.0f, (float)w, (float)h, 0.0f);
    glUniformMatrix4fv(glGetUniformLocation(fontShader.getShaderID(),
    "u_viewProj"), 1, GL_FALSE, &ortho[0][0]); glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    font.drawString(20.0f, 40.0f, camera.getPositionString().c_str(),
    &fontShader); font.drawString(20.0f, 60.0f, camera.getViewString().c_str(),
    &fontShader); font.drawString(20.0f, 80.0f,
    camera.getLookAtString().c_str(), &fontShader); string fpsString = "FPS: ";
    fpsString.append(to_string(fps));
    font.drawString(20.0f, 20.0f, fpsString.c_str(), &fontShader);

    fontShader.unbind();
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    */

#ifdef _DEBUG

    ImGui::Begin("Objects");
    for (Object *object : objects) {
        object->renderDebugUI();
    }
    for (WindowSurface *window : windows) {
        window->renderDebugUI();
    }
    ImGui::End();

    ImGui::Begin("Info");
    ImGui::Text("Frametime: %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);
    ImGui::Text("%s", camera.getPositionString().c_str());
    ImGui::Text("%s", camera.getViewString().c_str());
    ImGui::Text("%s", camera.getLookAtString().c_str());
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

    if (!SDL_GL_SwapWindow(window)) {
        cout << SDL_GetError() << endl;
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return running;
}
