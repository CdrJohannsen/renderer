
#include <SDL2/SDL.h>
#define GLEW_STATIC
#include <GL/glew.h>
#define SDL_MAIN_HANDLED
//#include <GL/gl.h>
//#include <SDL2/SDL_opengl.h>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include "vect.h"
#include "vert.h"
#include "face.h"
#include "matr.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
//#include "parseOBJ.h"
#include "shader.h"
#include "framebuffer.h"
#include "floating_camera.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
//#define STB_TRUETYPE_IMPLEMENTATION
//#include <stb/stb_truetype.h>
#include "light.h"
#include "mesh.h"
#include "font.h"
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
using namespace std;

#define WIDTH 1920.0f
#define HEIGTH 1080.0f
#define RATIO 1920.0f/1080.0f
#define SCALE 50
#define NEAR 0.1f
#define FAR 1000.0f
#define FOV 90.0f
#define FOVRAD tan(FOV*0.5f/180.0f*M_PI)

bool buttonW=false;
bool buttonA=false;
bool buttonS=false;
bool buttonD=false;
bool buttonSHIFT=false;
bool buttonSPACE=false;
bool buttonCTRL=false;

float cameraSpeed = 6.0f;

bool handleInput(SDL_Event &e, FloatingCamera &camera){
    while (SDL_PollEvent(&e)) {
        if(e.type == SDL_QUIT)
            return false;
        else if(e.type == SDL_KEYDOWN){
            if (SDL_GetRelativeMouseMode()) {
                switch(e.key.keysym.sym) {
                    case SDLK_w:
                        buttonW = true;
                        break;
                    case SDLK_s:
                        buttonS = true;
                        break;
                    case SDLK_a:
                        buttonA = true;
                        break;
                    case SDLK_d:
                        buttonD = true;
                        break;
                    case SDLK_SPACE:
                        buttonSPACE = true;
                        break;
                    case SDLK_LSHIFT:
                        buttonSHIFT = true;
                        break;
                    case SDLK_LCTRL:
                        buttonCTRL = true;
                        break;
                }
            }
            if (e.key.keysym.sym == SDLK_ESCAPE && SDL_GetRelativeMouseMode()) {
                SDL_SetRelativeMouseMode(SDL_FALSE);
            }
        }
        else if(e.type == SDL_KEYUP){
            switch(e.key.keysym.sym) {
                case SDLK_w:
                    buttonW = false;
                    break;
                case SDLK_s:
                    buttonS = false;
                    break;
                case SDLK_a:
                    buttonA = false;
                    break;
                case SDLK_d:
                    buttonD = false;
                    break;
                case SDLK_SPACE:
                    buttonSPACE = false;
                    break;
                case SDLK_LSHIFT:
                    buttonSHIFT = false;
                    break;
                case SDLK_LCTRL:
                    buttonCTRL = false;
                    break;
            }
        }
        else if(e.type == SDL_MOUSEMOTION){
            if(SDL_GetRelativeMouseMode()){
                camera.onMouseMove(e.motion.xrel,e.motion.yrel);
            }
        }
        else if(e.type == SDL_MOUSEBUTTONDOWN){
            if (e.button.button == SDL_BUTTON_LEFT){
                SDL_SetRelativeMouseMode(SDL_TRUE);
            }
        }
    }
    return true;
}

void handleMovement(FloatingCamera &camera, float &delta){
    if(buttonW){
        camera.moveFront(delta * cameraSpeed);
    }
    if(buttonS){
        camera.moveFront(delta * -cameraSpeed);
    }
    if(buttonA){
        camera.moveSide(delta * -cameraSpeed);
    }
    if(buttonD){
        camera.moveSide(delta * cameraSpeed);
    }
    if(buttonSPACE){
        camera.moveUp(delta * cameraSpeed);
    }
    if(buttonSHIFT){
        camera.moveUp(delta * -cameraSpeed);
    }
    if(buttonCTRL){
        cameraSpeed = 32.0f;
    }
    if(!buttonCTRL & (cameraSpeed == 32.0f)){
        cameraSpeed = 6.0f;
    }
}
void OpenGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,GLsizei length, const GLchar* message,const void* userParam){
    if (severity == GL_DEBUG_SEVERITY_HIGH || severity == GL_DEBUG_SEVERITY_MEDIUM){

    }
    cout << "[OpenGL Error] " << message << endl;
}

int main(int argc,char** argv)
{
    if (argc <= 1){
        cout << "Usage" << argv[0] << " [FILE]" << endl;
    }
    SDL_Window* window = nullptr;
    SDL_Event e;
    SDL_Init(SDL_INIT_EVERYTHING);

#ifdef _DEBUG
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS,SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

    uint32_t flags = SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE;

    SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" );
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    window = SDL_CreateWindow("Renderer", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,1920,1080, flags);
    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    GLenum err = glewInit();
    if (err != GLEW_OK && err != 4){
        cout << "Error: " << glewGetErrorString(err) << endl;
        cout << err << endl;
        cin.get();
        //return -1;
    }

    cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;

    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_BUFFER_SIZE, 32 );
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_GL_SetSwapInterval(-1);

    SDL_SetRelativeMouseMode(SDL_TRUE);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

#ifdef _DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(OpenGLDebugCallback,0);
#endif

    Shader fontShader("shaders/font.vert","shaders/font.frag");
    Shader postProcessShader("shaders/post.vert","shaders/post.frag");
    Shader shader("shaders/basic.vert","shaders/basic.frag");
    shader.bind();

    glm::vec3 sunColor(0.2f, 0.2f, 0.2f);
    glm::vec3 sunDirection(0.0f, -1.0f, 0.0f);
    DirLight sun(&shader,sunDirection,sunColor,0.4f);

    glm::vec3 pointLightColor(0.0f, 1.0f, 1.0f);
    glm::vec4 pointLightPosition(0.0f,4.5f,0.0f,1.0f);
    PointLight point(&shader,pointLightPosition,pointLightColor,0.2f,0.017f,0.0018f);

    glm::vec3 spotLightColor(0.0f, 0.0f, 0.0f);
    glm::vec4 spotLightPosition(7.0f,7.0f,7.0f,1.0f);
    glm::vec3 spotLightDirection(5.0f,8.0f,8.0f);
    SpotLight spot(&shader,spotLightPosition,spotLightDirection,spotLightColor,0.1f,0.996f,0.995f);

    Font font;
    font.initFont("ressources/fonts/liberation-sans/LiberationSans-Bold.ttf");

    Model modelTree;
    modelTree.init(argv[1],&shader);

    uint64_t perfCounterFrequency = SDL_GetPerformanceFrequency();
    uint64_t lastCounter = SDL_GetPerformanceCounter();
    float delta = 0.0f; 
    float time = 0;

    glm::mat4 model = glm::mat4(1.0f);
    //model = glm::scale(model,glm::vec3(0.1f));

    FloatingCamera camera(90.0f,1920.0f,1080.0f);
    //camera.translate(glm::vec3(0.0f,0.0f,5.0f));
    camera.update();

    glm::mat4 modelViewProj = model * camera.getViewProj();


    int modelViewLocation = glGetUniformLocation(shader.getShaderID(),"u_modelView");
    int invModelViewLocation = glGetUniformLocation(shader.getShaderID(),"u_invModelView");
    int modelViewProjMatrixLocation = glGetUniformLocation(shader.getShaderID(),"u_modelViewProj");

    int textureUniformLocation = glGetUniformLocation(shader.getShaderID(),"u_texture");
    if (textureUniformLocation != -1){
        glUniform1i(textureUniformLocation,0);
    }
    int w,h;
    SDL_GetWindowSize(window, &w, &h);
    FrameBuffer framebuffer;
    framebuffer.create(w,h);

    glClearColor(0.01f,0.01f,0.01f,1.0f);
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    uint32_t fps = 0;
    bool running=true;
    while(running) {
        time += delta;
        running = handleInput(e,camera);
        handleMovement(camera,delta);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        camera.update();
        shader.bind();

        framebuffer.bind();

        //model=glm::rotate(model,1.0f*delta,glm::vec3(0,1,0));

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        modelViewProj = camera.getViewProj() * model;
        glm::mat4 modelView = camera.getView() * model;
        glm::mat4 invModelView = glm::transpose(glm::inverse(modelView));

        sun.update(camera.getView());
        point.update(camera.getView(),glm::mat4(1.0f));
        spot.update(camera.getView(),glm::mat4(1.0f));

        glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE,&modelView[0][0]);
        glUniformMatrix4fv(invModelViewLocation, 1, GL_FALSE,&invModelView[0][0]);
        glUniformMatrix4fv(modelViewProjMatrixLocation, 1, GL_FALSE,&modelViewProj[0][0]);
        glActiveTexture(GL_TEXTURE0);

        modelTree.render();
        shader.unbind();
        framebuffer.unbind();

        postProcessShader.bind();
        GLuint pPTextureLocation = glGetUniformLocation(postProcessShader.getShaderID(), "u_texture");
        GLuint pPDepthLocation = glGetUniformLocation(postProcessShader.getShaderID(), "u_depth");

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, framebuffer.getTextureID(0));
        glUniform1i(pPTextureLocation,0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, framebuffer.getTextureID(1));
        glUniform1i(pPDepthLocation,1);

        glActiveTexture(GL_TEXTURE0);
        glDrawArrays(GL_TRIANGLES,0,3);
        postProcessShader.unbind();

        fontShader.bind();

        int w,h;
        SDL_GetWindowSize(window, &w, &h);
        glm::mat4 ortho = glm::ortho(0.0f, (float)w, (float)h, 0.0f);
        glUniformMatrix4fv(glGetUniformLocation(fontShader.getShaderID(), "u_viewProj"), 1, GL_FALSE, &ortho[0][0]);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);

        font.drawString(20.0f, 40.0f, camera.getPositionString().c_str(), &fontShader);
        font.drawString(20.0f, 60.0f, camera.getViewString().c_str(), &fontShader);
        font.drawString(20.0f, 80.0f, camera.getLookAtString().c_str(), &fontShader);
        string fpsString = "FPS: ";
        fpsString.append(to_string(fps));
        font.drawString(20.0f, 20.0f, fpsString.c_str(), &fontShader);

        fontShader.unbind();
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        SDL_GL_SwapWindow(window);

        uint64_t endCounter = SDL_GetPerformanceCounter();
        uint64_t counterElapsed = endCounter-lastCounter;
        delta = ((float)counterElapsed) / ((float)perfCounterFrequency);
        fps = (uint32_t)((float)perfCounterFrequency / (float)counterElapsed);
        //cout << "FPS: " << fps << endl;
        lastCounter=endCounter;
    }
    framebuffer.destroy();

    return 0;
}
