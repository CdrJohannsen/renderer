#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <cmath>
#include <iostream>
using namespace std;

float getCos(int degree) {
    return cos((M_PI/180)*degree);
}

float getSin(int degree) {
    return sin((M_PI/180)*degree);
}

int main()
{
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Event e;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(1920,1080,0,&window, &renderer);

    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer,255,255,255,255);
    
    int rx = 90;
    int ry = 0;
    int rz = 0;
    int size = 200;
    bool running=true;
    while(running) {
        while (SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT)
                running = false;
            else if(e.type == SDL_KEYDOWN)
            {
                if(SDLK_d == e.key.keysym.sym) {
                    ry += 2;
                }
                else if(SDLK_a == e.key.keysym.sym) {
                    ry -= 2;
                }
                else if(SDLK_w == e.key.keysym.sym) {
                    rx += 2;
                }
                else if(SDLK_s == e.key.keysym.sym) {
                    rx -= 2;
                }
                else if(SDLK_e == e.key.keysym.sym) {
                    rz += 2;
                }
                else if(SDLK_q == e.key.keysym.sym) {
                    rz -= 2;
                }
                else if(SDLK_ESCAPE == e.key.keysym.sym) {
                    running=false;
                }
                break;
            }
        }
    
        const std::vector< SDL_Vertex > verts =
        {
            { SDL_FPoint{ 960+getCos(ry)*getCos(0+rz)*size, 540+getSin(rx)*getSin(0+rz)*size }, SDL_Color{ 255,255,255,255 }, SDL_FPoint{ 0 }, },
            { SDL_FPoint{ 960+getCos(ry)*getCos(120+rz)*size, 540+getSin(rx)*getSin(120+rz)*size }, SDL_Color{ 255,255,255,255 }, SDL_FPoint{ 0 }, },
            { SDL_FPoint{ 960+getCos(ry)*getCos(240+rz)*size, 540+getSin(rx)*getSin(240+rz)*size }, SDL_Color{ 255,255,255,255}, SDL_FPoint{ 0 }, }
        };

        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_RenderClear(renderer);
        SDL_RenderGeometry(renderer,nullptr,verts.data(),verts.size(),nullptr,0);
        SDL_RenderDrawPoint(renderer,960,540);

        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }

    return 0;
}

