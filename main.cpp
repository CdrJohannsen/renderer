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
#define SCALE 50
#define NEAR 0.1f
#define FAR 1000.0f
#define FOV 90.0f
#define FOVRAD 1.0f/tan(90*0.5f/180.0f*M_PI)


void MatrixMultiply(Vect &i,Vect &o,Matrix4x4 &m){
    o.x=i.x*m.m[0][0]+i.y*m.m[1][0]+i.z*m.m[2][0]+m.m[3][0];
    o.y=i.x*m.m[0][1]+i.y*m.m[1][1]+i.z*m.m[2][1]+m.m[3][1];
    o.z=i.x*m.m[0][2]+i.y*m.m[1][2]+i.z*m.m[2][2]+m.m[3][2];;
    float w=i.x*m.m[0][3]+i.y*m.m[1][3]+i.z*m.m[2][3]+m.m[3][3];

    if (w!=0){o.x/=w;o.y/=w;o.z/=w;}
}

Vect cr={-1,-1,-1};
Vect c={2,2,2};

float getCos(int degree) {
    return cos((M_PI/180)*degree);
}

float getSin(int degree) {
    return sin((M_PI/180)*degree);
}
float dot(Vect a, Vect b){
    return a.x*b.x+a.y+b.y+a.z+b.z;
}

void renderTriangle(SDL_Renderer* renderer,vector<SDL_Vertex> v){
    cout << v[0].position.x << " " << v[1].position.y << endl;
    SDL_SetRenderDrawColor(renderer,255,255,255,255);
    SDL_RenderDrawLine(renderer,v[0].position.x,v[0].position.y,v[1].position.x,v[1].position.y);
    SDL_RenderDrawLine(renderer,v[1].position.x,v[1].position.y,v[2].position.x,v[2].position.y);
    SDL_RenderDrawLine(renderer,v[2].position.x,v[2].position.y,v[0].position.x,v[0].position.y);
}

tuple<float,float> convert3Dto2D(Vect v){
    /*
    Matrix4x4 projMat;
    projMat.m[0][0]=RATIO/FOVRAD;
    projMat.m[1][1]=FOVRAD;
    projMat.m[2][2]=FAR/(FAR-NEAR);
    projMat.m[3][2]=(-FAR*NEAR)/(FAR-NEAR);
    projMat.m[2][3]=1.0f;
    projMat.m[3][3]=0.0f;
    Matrix4x4 rotxMat;
    rotxMat.m[0][0]=1;
    rotxMat.m[1][1]=getCos(cx);
    rotxMat.m[2][2]=getCos(cx);
    rotxMat.m[1][2]=getSin(cx);
    rotxMat.m[2][1]=(-getSin(cx));
    Matrix4x4 rotyMat;
    rotyMat.m[0][0]=getCos(cy);
    rotyMat.m[1][1]=1;
    rotyMat.m[2][2]=getCos(cy);
    rotyMat.m[0][2]=getSin(cy);
    rotyMat.m[2][0]=(-getSin(cy));
    Matrix4x4 rotzMat;
    rotzMat.m[0][0]=getCos(cz);
    rotzMat.m[1][1]=1;
    rotzMat.m[2][2]=getCos(cz);
    rotzMat.m[0][2]=getSin(cz);
    rotzMat.m[2][0]=(-getSin(cz));
    Vect o1;
    Vect o2;
    Vect o3;
    Vect o4;
    MatrixMultiply(v,o1,projMat);
    MatrixMultiply(o1,o2,rotyMat);
    MatrixMultiply(o2,o3,rotxMat);
    MatrixMultiply(o3,o4,rotzMat);
    float dx=getCos(cr.y)*(getSin(cr.z)*diff.y-getCos(cr.z)*diff.x)-getSin(cr.y)*diff.z;
    float dy=getSin(cr.x)*(getCos(cr.y)*diff.z+getSin(cr.y)*(getSin(cr.z)*diff.y+getCos(cr.z)*diff.x)+getCos(cr.x)*(getCos(cr.z)*diff.y-getSin(cr.z)*diff.x));
    float dz=getCos(cr.x)*(getCos(cr.y)*diff.z+getSin(cr.y)*(getSin(cr.z)*diff.y-getCos(cr.z)*diff.x)-getSin(cr.x)*(getCos(cr.z)*diff.y-getSin(cr.z)*diff.x));
    */
    Vect diff=v-c;
    float dx=getCos(cr.y)*(getSin(cr.z)*diff.y+getCos(cr.z)*diff.x)-getSin(cr.y)*diff.z;
    float dy=getSin(cr.x)*(getCos(cr.y)*diff.z+getSin(cr.y)*(getSin(cr.z)*diff.y+getCos(cr.z)*diff.x)+getCos(cr.x)*(getCos(cr.z)*diff.y-getSin(cr.z)*diff.x));
    float dz=getCos(cr.x)*(getCos(cr.y)*diff.z+getSin(cr.y)*(getSin(cr.z)*diff.y+getCos(cr.z)*diff.x)-getSin(cr.x)*(getCos(cr.z)*diff.y-getSin(cr.z)*diff.x));
    float x = (90/dz)*dx;
    float y = (90/dz)*dy;
    return make_tuple(x,y);
}

void drawFace(SDL_Renderer** renderer,Face &face){
    float x1,y1,x2,y2,x3,y3;
    tie(x1,y1) = convert3Dto2D(verticies[face.vert_a]);
    tie(x2,y2) = convert3Dto2D(verticies[face.vert_b]);
    tie(x3,y3) = convert3Dto2D(verticies[face.vert_c]);
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
    //SDL_RenderGeometry(*renderer,nullptr,verts.data(),verts.size(),nullptr,0);
    renderTriangle(*renderer,verts);
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
                    c.x += 1;
                }
                else if(SDLK_a == e.key.keysym.sym) {
                    c.x -= 1;
                }
                else if(SDLK_w == e.key.keysym.sym) {
                    c += cr;
                }
                else if(SDLK_s == e.key.keysym.sym) {
                    c -= cr;
                }
                else if(SDLK_e == e.key.keysym.sym) {
                    c.y += 1;
                }
                else if(SDLK_q == e.key.keysym.sym) {
                    c.y -= 1;
                }
                else if(SDLK_RIGHT == e.key.keysym.sym) {
                    cr.y += 1;
                }
                else if(SDLK_LEFT == e.key.keysym.sym) {
                    cr.y -= 1;
                }
                else if(SDLK_UP == e.key.keysym.sym) {
                    cr.x += 1;
                }
                else if(SDLK_DOWN == e.key.keysym.sym) {
                    cr.x -= 1;
                }
                else if(SDLK_ESCAPE == e.key.keysym.sym) {
                    running=false;
                }
                break;
            }
        }
        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_RenderClear(renderer);
        //cr = cr/cr.length();
        for (Face &face:faces){
            drawFace(&renderer,face);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(10);

    }
    return 0;
}
