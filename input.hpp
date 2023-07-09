#pragma once
#include <SDL2/SDL.h>
#include "floating_camera.hpp"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

struct{
    bool buttonW=false;
    bool buttonA=false;
    bool buttonS=false;
    bool buttonD=false;
    bool buttonSHIFT=false;
    bool buttonSPACE=false;
    bool buttonCTRL=false;
}pressedButtons;

float cameraSpeed = 6.0f;

bool handleInput(SDL_Event &e, FloatingCamera &camera){
    while (SDL_PollEvent(&e)) {
        ImGui_ImplSDL2_ProcessEvent(&e);
        if(e.type == SDL_QUIT)
            return false;
        else if(e.type == SDL_KEYDOWN){
            if (e.key.keysym.sym == SDLK_ESCAPE && !SDL_GetRelativeMouseMode()) {
                SDL_SetRelativeMouseMode(SDL_TRUE);
                cout << "test" << endl;
            }
            else if (e.key.keysym.sym == SDLK_ESCAPE && SDL_GetRelativeMouseMode()) {
                SDL_SetRelativeMouseMode(SDL_FALSE);
            }
            if (SDL_GetRelativeMouseMode()) {
                switch(e.key.keysym.sym) {
                    case SDLK_w:
                        pressedButtons.buttonW = true;
                        break;
                    case SDLK_s:
                        pressedButtons.buttonS = true;
                        break;
                    case SDLK_a:
                        pressedButtons.buttonA = true;
                        break;
                    case SDLK_d:
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
        }
        else if(e.type == SDL_KEYUP){
            switch(e.key.keysym.sym) {
                case SDLK_w:
                    pressedButtons.buttonW = false;
                    break;
                case SDLK_s:
                    pressedButtons.buttonS = false;
                    break;
                case SDLK_a:
                    pressedButtons.buttonA = false;
                    break;
                case SDLK_d:
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
        }
        else if(e.type == SDL_MOUSEMOTION){
            if(SDL_GetRelativeMouseMode()){
                camera.onMouseMove(e.motion.xrel,e.motion.yrel);
            }
        }
        // else if(e.type == SDL_MOUSEBUTTONDOWN){
            // if (e.button.button == SDL_BUTTON_LEFT){
                // SDL_SetRelativeMouseMode(SDL_TRUE);
            // }
        // }
    }
    return true;
}

void handleMovement(FloatingCamera &camera, float &delta){
    if(pressedButtons.buttonW){
        camera.moveFront(delta * cameraSpeed);
    }
    if(pressedButtons.buttonS){
        camera.moveFront(delta * -cameraSpeed);
    }
    if(pressedButtons.buttonA){
        camera.moveSide(delta * -cameraSpeed);
    }
    if(pressedButtons.buttonD){
        camera.moveSide(delta * cameraSpeed);
    }
    if(pressedButtons.buttonSPACE){
        camera.moveUp(delta * cameraSpeed);
    }
    if(pressedButtons.buttonSHIFT){
        camera.moveUp(delta * -cameraSpeed);
    }
    if(pressedButtons.buttonCTRL){
        cameraSpeed = 32.0f;
    }
    if(!pressedButtons.buttonCTRL & (cameraSpeed == 32.0f)){
        cameraSpeed = 6.0f;
    }
}

