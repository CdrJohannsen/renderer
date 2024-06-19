
#include <stdlib.h>

#include <iostream>
using namespace std;
#include <SDL2/SDL.h>
#define GLEW_STATIC
#include <GL/glew.h>
#define SDL_MAIN_HANDLED
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl2.h"
#define STB_IMAGE_IMPLEMENTATION
#ifdef _DEBUG
#include "imgui.h"
#endif
// #include <GL/gl.h>
// #include <SDL2/SDL_opengl.h>
#include <stb/stb_image.h>

#include <cmath>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "cubemap.hpp"
#include "floating_camera.hpp"
#include "font.hpp"
#include "framebuffer.hpp"
#include "gbuffer.hpp"
#include "input.hpp"
#include "object.hpp"
#include "renderUtils.hpp"
#include "shader.hpp"

#define WIDTH 1920.0f
#define HEIGTH 1080.0f
#define RATIO 1920.0f / 1080.0f

void OpenGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message,
                         const void *userParam) {
    if (severity == GL_DEBUG_SEVERITY_HIGH || severity == GL_DEBUG_SEVERITY_MEDIUM) {
    }
    cout << "[OpenGL Error] " << message << endl;
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        cout << "Usage" << argv[0] << " [FILE]" << endl;
        exit(1);
    }
    SDL_Window *window;
    SDL_Event e;
    SDL_Init(SDL_INIT_EVERYTHING);

#ifdef _DEBUG
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

    uint32_t flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN;

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    window = SDL_CreateWindow("Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, flags);
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        cout << "No context" << endl;
    }

    GLenum err = glewInit();
    if (err != GLEW_OK && err != 4) {
        cout << "Error: " << glewGetErrorString(err) << endl;
        cout << err << endl;
        // cin.get();
        return -1;
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

    SDL_SetRelativeMouseMode(SDL_TRUE);

#ifdef _DEBUG
    // IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, glContext);
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

    Shader fontShader("shaders/font.vert", "shaders/font.frag");
    Shader postProcessShader("shaders/post.vert", "shaders/post.frag");
    Shader skyboxShader("shaders/sky.vert", "shaders/sky.frag");
    Shader shader("shaders/basic.vert", "shaders/basic.frag");
    shader.bind();
    Shader gBufferShader("shaders/gBuffer.vert", "shaders/gBuffer.frag");
    Shader deferredShader("shaders/deferred.vert", "shaders/deferred.frag");

    /*
       glm::vec3 sunColor(0.2f, 0.2f, 0.2f);
       glm::vec3 sunDirection(0.0f, -1.0f, 0.0f);
       DirLight sun(&shader,sunDirection,sunColor,0.4f);

       glm::vec3 pointLightColor(0.0f, 1.0f, 1.0f);
       glm::vec4 pointLightPosition(0.0f,4.5f,0.0f,1.0f);
       PointLight
       point(&shader,pointLightPosition,pointLightColor,0.2f,0.017f,0.0018f);

       glm::vec3 spotLightColor(0.0f, 0.0f, 0.0f);
       glm::vec4 spotLightPosition(7.0f,7.0f,7.0f,1.0f);
       glm::vec3 spotLightDirection(5.0f,8.0f,8.0f);
       SpotLight
       spot(&shader,spotLightPosition,spotLightDirection,spotLightColor,0.1f,0.996f,0.995f);
       */

    Font font;
    font.initFont((char *)"ressources/fonts/liberation-sans/LiberationSans-Bold.ttf");

    // Model modelTree;
    // modelTree.init(argv[1],&shader);
    Object testfield("Testfield",argv[1], &gBufferShader, &deferredShader, {0, 0, 0});

    FloatingCamera camera(90.0f, 1920.0f, 1080.0f);
    // camera.translate(glm::vec3(0.0f,0.0f,5.0f));
    camera.update();

    // int textureUniformLocation =
    // glGetUniformLocation(shader.getShaderID(),"u_texture"); if
    // (textureUniformLocation != -1){ glUniform1i(textureUniformLocation,0);
    // }
    int w, h;
    SDL_GetWindowSize(window, &w, &h);

    unsigned int cubemapTexture = loadCubemap(getCubemapTextures());
    GLuint skyboxVAO = getSkyboxVAO();

    FrameBuffer framebuffer;
    framebuffer.create(w, h);

    GBuffer gBuffer;
    gBuffer.create(w, h);
    deferredShader.bind();
    deferredShader.setInt("gPosition", 0);
    deferredShader.setInt("gNormal", 1);
    deferredShader.setInt("gColorSpec", 2);
    deferredShader.setInt("gEmissive", 3);

    glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
    // glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    uint32_t fps = 0;
    uint64_t perfCounterFrequency = SDL_GetPerformanceFrequency();
    uint64_t lastCounter = SDL_GetPerformanceCounter();
    float delta = 0.0f;
    float time = 0;
    uint64_t endCounter = SDL_GetPerformanceCounter();
    uint64_t counterElapsed = endCounter - lastCounter;
    bool running = true;
    while (running) {
        time += delta;
        running = handleInput(e, camera);
        handleMovement(camera, delta);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        camera.update();

#ifdef _DEBUG
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
#endif

        // testfield.move(0.0f,delta,0.0f);

        // model=glm::rotate(model,1.0f*delta,glm::vec3(0,1,0));

        // sun.update(camera.getView());
        // point.update(camera.getView(),glm::mat4(1.0f));
        // spot.update(camera.getView(),glm::mat4(1.0f));

        // glActiveTexture(GL_TEXTURE0);

        // modelTree.render(camera.getView(),glm::mat4(1.0f));
        gBuffer.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        gBufferShader.bind();

        testfield.render(camera);
        gBuffer.unbind();
        gBufferShader.unbind();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        deferredShader.bind();
        gBuffer.bindTexture();
        testfield.updateLights(camera);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderQuad();
        deferredShader.unbind();
        // gBuffer.blitFramebuffer();

        // Skybox

        glDepthFunc(GL_LEQUAL);
        glm::mat4 projection = camera.getProj();
        glm::mat4 view = glm::mat4(glm::mat3(camera.getView()));
        skyboxShader.bind();
        skyboxShader.setMat4("u_view", view);
        skyboxShader.setMat4("u_projection", projection);
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        skyboxShader.unbind();
        glDepthFunc(GL_LESS);

        // framebuffer.unbind();

        /*
        // Post Processing

        postProcessShader.bind();
        GLuint pPTextureLocation =
        glGetUniformLocation(postProcessShader.getShaderID(), "u_texture"); GLuint
        pPDepthLocation = glGetUniformLocation(postProcessShader.getShaderID(),
        "u_depth");

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, framebuffer.getTextureID(0));
        glUniform1i(pPTextureLocation,0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, framebuffer.getTextureID(1));
        glUniform1i(pPDepthLocation,1);

        glActiveTexture(GL_TEXTURE0);
        glDrawArrays(GL_TRIANGLES,0,3);
        postProcessShader.unbind();
        */
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
        testfield.renderDebugUI();
        testfield.move(0, 0, 0);
        ImGui::End();

        ImGui::Begin("Info");
        ImGui::Text("Frametime: %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::Text(camera.getPositionString().c_str());
        ImGui::Text(camera.getViewString().c_str());
        ImGui::Text(camera.getLookAtString().c_str());
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

        SDL_GL_SwapWindow(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        endCounter = SDL_GetPerformanceCounter();
        counterElapsed = endCounter - lastCounter;
        delta = ((float)counterElapsed) / ((float)perfCounterFrequency);
        fps = (uint32_t)((float)perfCounterFrequency / (float)counterElapsed);
        // cout << "End: " << fixed << delta << endl;
        // cout << "FPS: " << fps << endl;
        lastCounter = endCounter;
        // return 0;
    }
#ifdef _DEBUG
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
#endif
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    framebuffer.destroy();

    SDL_SetRelativeMouseMode(SDL_FALSE);
    return 0;
}
