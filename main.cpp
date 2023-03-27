#include <SDL2/SDL.h>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <tuple>
#include "vect.cpp"
#include "face.cpp"
#include "matr.h"
#include "parseOBJ.cpp"
#include <iostream>
using namespace std;

#define WIDTH 1920.0f
#define HEIGTH 1080.0f
#define RATIO 1080.0f/1920.0f
#define SCALE 25
#define NEAR 0.1f
#define FAR 1000.0f
#define FOV 90.0f
#define FOVRAD 1.0f/tan(2*0.5f/180.0f*M_PI)


void MatrixMultiply(Vect &i,Vect &o,Matrix4x4 &m){
    o.x=i.x*m.m[0][0]+i.y*m.m[1][0]+i.z*m.m[2][0]+m.m[3][0];
    o.y=i.x*m.m[0][1]+i.y*m.m[1][1]+i.z*m.m[2][1]+m.m[3][1];
    o.z=i.x*m.m[0][2]+i.y*m.m[1][2]+i.z*m.m[2][2]+m.m[3][2];
    float w=i.x*m.m[0][3]+i.y*m.m[1][3]+i.z*m.m[2][3]+m.m[3][3];

    if (w!=0){o.x/=w;o.y/=w;o.z/=w;}
}

int cx=2;
int cy=2;
int cz=2;
Vect cr={-1,-1,-1};

float getCos(int degree) {
    return cos((M_PI/180)*degree);
}

float getSin(int degree) {
    return sin((M_PI/180)*degree);
}
float dot(Vect a, Vect b){
    return a.x*b.x+a.y+b.y+a.z+b.z;
}

tuple<float,float> convert3Dto2D(Vect v){
    Matrix4x4 projMat;
    projMat.m[0][0]=RATIO/FOVRAD;
    projMat.m[1][1]=FOVRAD;
    projMat.m[2][2]=FAR/(FAR-NEAR);
    projMat.m[3][2]=(-FAR*NEAR)/(FAR-NEAR);
    projMat.m[2][3]=1.0f;
    projMat.m[3][3]=0.0f;
    Vect o;
    MatrixMultiply(v,o,projMat);
    return make_tuple(o.x,o.y);
}

void drawFace(SDL_Renderer** renderer,Face &face){
    float x1,y1,x2,y2,x3,y3;
    tie(x1,y1) = convert3Dto2D(verticies[face.vert_a]-2);
    tie(x2,y2) = convert3Dto2D(verticies[face.vert_b]-2);
    tie(x3,y3) = convert3Dto2D(verticies[face.vert_c]-2);
    cout << x1 << " " << y1 <<endl;
    cout << x2 << " " << y2 <<endl;
    cout << x3 << " " << y3 <<endl;
    cout << normals[face.norm_a] << endl;
    //if (dot(normals[face.norm_a],cr)>0){return;}
    const vector<SDL_Vertex> verts={
        {
            SDL_FPoint{960+x1,540+y1},
            SDL_Color{255,255,255,255},
            SDL_FPoint{0}
        },
        {
            SDL_FPoint{960+x2,540+y2},
            SDL_Color{255,255,0,255},
            SDL_FPoint{0}
        },
        {
            SDL_FPoint{960+x3,540+y3},
            SDL_Color{255,0,255,255},
            SDL_FPoint{0}
        },
    };
    SDL_RenderGeometry(*renderer,nullptr,verts.data(),verts.size(),nullptr,0);
    return;
}

int main()
{
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Event e;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(1920,1080,SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL,&window, &renderer);
    SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" );
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);

    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    SDL_RenderClear(renderer);

    //SDL_SetRenderDrawColor(renderer,255,255,255,255);

    vector<Face> faces=parse();
    bool running=true;
    while(running) {
        while (SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT)
                running = false;
            else if(e.type == SDL_KEYDOWN)
            {
                if(SDLK_d == e.key.keysym.sym) {
                    cx += 1;
                }
                else if(SDLK_a == e.key.keysym.sym) {
                    cx -= 1;
                }
                else if(SDLK_w == e.key.keysym.sym) {
                    cy += 1;
                }
                else if(SDLK_s == e.key.keysym.sym) {
                    cy -= 1;
                }
                else if(SDLK_e == e.key.keysym.sym) {
                    cz += 1;
                }
                else if(SDLK_q == e.key.keysym.sym) {
                    cz -= 1;
                }
                else if(SDLK_ESCAPE == e.key.keysym.sym) {
                    running=false;
                }
                break;
            }
        }
        SDL_RenderClear(renderer);
        for (Face &face:faces){
            drawFace(&renderer,face);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(10);

    }
    return 0;
}
