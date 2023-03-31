#include <SDL2/SDL.h>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
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
#define FOVRAD 1.0f/tan(FOV*0.5f/180.0f*M_PI)


Matrix4x4 MatrixMultiply(Matrix4x4 &m1, Matrix4x4 &m2) {
    Matrix4x4 matrix;
    for (int c = 0; c < 4; c++)
        for (int r = 0; r < 4; r++)
            matrix.m[r][c] = m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] + m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];
    return matrix;
}

Matrix4x4 VectorToMatrix(Vect &v) {
    Matrix4x4 matrix;
    matrix.m[0][0] = 1.0f;
    matrix.m[1][1] = 1.0f;
    matrix.m[2][2] = 1.0f;
    matrix.m[3][3] = 1.0f;
    matrix.m[3][0] = v.x;
    matrix.m[3][1] = v.y;
    matrix.m[3][2] = v.z;
    return matrix;
}

Vect cr={0,0,0};
Vect cp={0,0,0};

float getCos(int degree) {
    return cos((M_PI/180)*degree);
}

float getSin(int degree) {
    return sin((M_PI/180)*degree);
}
float dot(Vect a, Vect b){
    return a.x*b.x+a.y+b.y+a.z+b.z;
}

Vect angleToVect(Vect a){
    // converts 3 angles (filled into a Vector, but not actually a vector) and converts them to a Vector
    Vect out;
    out.x=getSin(a.x)*getCos(a.y)*getCos(a.z);
    out.y=getSin(a.y)*getCos(a.z)*getCos(a.x);
    out.z=getSin(a.z)*getCos(a.x)*getCos(a.y);
    return out;
}


void renderTriangle(SDL_Renderer* renderer,vector<SDL_Vertex> v){
    cout << v[0].position.x << " " << v[1].position.y << endl;
    SDL_SetRenderDrawColor(renderer,255,255,255,255);
    SDL_RenderDrawLine(renderer,v[0].position.x,v[0].position.y,v[1].position.x,v[1].position.y);
    SDL_RenderDrawLine(renderer,v[1].position.x,v[1].position.y,v[2].position.x,v[2].position.y);
    SDL_RenderDrawLine(renderer,v[2].position.x,v[2].position.y,v[0].position.x,v[0].position.y);
}

Matrix4x4 convert3Dto2D(Vect v){
    Matrix4x4 projMat;
    Vect diff=cp-v;
    Matrix4x4 m = VectorToMatrix(diff);
    projMat.m[0][0]=RATIO/FOVRAD;
    projMat.m[1][1]=FOVRAD;
    projMat.m[2][2]=FAR/(FAR-NEAR);
    projMat.m[3][2]=(-FAR*NEAR)/(FAR-NEAR);
    projMat.m[2][3]=1.0f;
    projMat.m[3][3]=0.0f;
    Matrix4x4 rotxMat;
    rotxMat.m[0][0]=1;
    rotxMat.m[1][1]=getCos(cr.x);
    rotxMat.m[1][2]=getSin(cr.x);
    rotxMat.m[2][1]=(-getSin(cr.x));
    rotxMat.m[2][2]=getCos(cr.x);
    rotxMat.m[3][3]=1;
    Matrix4x4 rotyMat;
    rotyMat.m[0][0]=getCos(cr.y);
    rotyMat.m[1][1]=1;
    rotyMat.m[2][2]=getCos(cr.y);
    rotyMat.m[0][2]=(-getSin(cr.y));
    rotyMat.m[2][0]=getSin(cr.y);
    rotyMat.m[3][3]=1;
    Matrix4x4 rotzMat;
    rotzMat.m[0][0]=getCos(cr.z);
    rotzMat.m[0][1]=getSin(cr.z);
    rotzMat.m[1][0]=(-getSin(cr.z));
    rotzMat.m[1][1]=getCos(cr.z);
    rotzMat.m[2][2]=1;
    rotzMat.m[3][3]=1;
    Vect o1;
    Vect o2;
    Vect o3;
    Vect o4;
    Matrix4x4 o = MatrixMultiply(m,projMat);
    m = MatrixMultiply(o,rotzMat);
    o = MatrixMultiply(m,rotxMat);
    m = MatrixMultiply(o,rotyMat);
    /*
       float dx=getCos(cr.y)*(getSin(cr.z)*diff.y-getCos(cr.z)*diff.x)-getSin(cr.y)*diff.z;
       float dy=getSin(cr.x)*(getCos(cr.y)*diff.z+getSin(cr.y)*(getSin(cr.z)*diff.y+getCos(cr.z)*diff.x)+getCos(cr.x)*(getCos(cr.z)*diff.y-getSin(cr.z)*diff.x));
       float dz=getCos(cr.x)*(getCos(cr.y)*diff.z+getSin(cr.y)*(getSin(cr.z)*diff.y-getCos(cr.z)*diff.x)-getSin(cr.x)*(getCos(cr.z)*diff.y-getSin(cr.z)*diff.x));

       Vect diff=c-v;
       float dx=getCos(cr.y)*(getSin(cr.z)*diff.y+getCos(cr.z)*diff.x)-getSin(cr.y)*diff.z;
       float dy=getSin(cr.x)*(getCos(cr.y)*diff.z+getSin(cr.y)*(getSin(cr.z)*diff.y+getCos(cr.z)*diff.x)+getCos(cr.x)*(getCos(cr.z)*diff.y-getSin(cr.z)*diff.x));
       float dz=getCos(cr.x)*(getCos(cr.y)*diff.z+getSin(cr.y)*(getSin(cr.z)*diff.y+getCos(cr.z)*diff.x)-getSin(cr.x)*(getCos(cr.z)*diff.y-getSin(cr.z)*diff.x));
       float x = (2/dz)*dx;
       float y = (2/dz)*dy;
       */

    return m;
}

void drawFace(SDL_Renderer** renderer,Face &face){
    Matrix4x4 a = convert3Dto2D(verticies[face.vert_a]);
    Matrix4x4 b = convert3Dto2D(verticies[face.vert_b]);
    Matrix4x4 c = convert3Dto2D(verticies[face.vert_c]);
    float x1 = a.m[3][0]*100;
    float x2 = b.m[3][0]*100;
    float x3 = c.m[3][0]*100;
    float y1 = a.m[3][1]*100;
    float y2 = b.m[3][1]*100;
    float y3 = c.m[3][1]*100;
    Vect to_point(a.m[3][0],a.m[3][1],a.m[3][2]);
    if (dot(normals[face.norm_a],cp-to_point)>0.0f){return;}
    const vector<SDL_Vertex> verts={
        {
            SDL_FPoint{960+x1,540+y1},
            SDL_Color{127.0f*(normals[face.norm_a].x+1.0f),127.0f*(normals[face.norm_a].y+1.0f),127.0f*(normals[face.norm_a].z+1.0f),255},
            SDL_FPoint{0}
        },
        {
            SDL_FPoint{960+x2,540+y2},
            SDL_Color{127.0f*(normals[face.norm_b].x+1.0f),127.0f*(normals[face.norm_b].y+1.0f),127.0f*(normals[face.norm_b].z+1.0f),255},
            SDL_FPoint{0}
        },
        {
            SDL_FPoint{960+x3,540+y3},
            SDL_Color{127.0f*(normals[face.norm_c].x+1.0f),127.0f*(normals[face.norm_c].y+1.0f),127.0f*(normals[face.norm_c].z+1.0f),255},
            SDL_FPoint{0}
        },
    };
    SDL_RenderGeometry(*renderer,nullptr,verts.data(),verts.size(),nullptr,0);
    //renderTriangle(*renderer,verts);
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
                if(SDLK_w == e.key.keysym.sym) {
                    cp.x -= getSin(cr.y);
                    cp.z -= getCos(cr.y);
                }
                else if(SDLK_s == e.key.keysym.sym) {
                    cp.x += getSin(cr.y);
                    cp.z += getCos(cr.y);
                }
                else if(SDLK_a == e.key.keysym.sym) {
                    cp.x += getSin(cr.y+90);
                    cp.z += getCos(cr.y+90);
                }
                else if(SDLK_d == e.key.keysym.sym) {
                    cp.x += getSin(cr.y-90);
                    cp.z += getCos(cr.y-90);
                }
                else if(SDLK_SPACE == e.key.keysym.sym) {
                    cp.y += 1;
                }
                else if(SDLK_LSHIFT == e.key.keysym.sym) {
                    cp.y -= 1;
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
                else if(SDLK_RCTRL == e.key.keysym.sym) {
                    cr.z += 1;
                }
                else if(SDLK_KP_0 == e.key.keysym.sym) {
                    cr.z -= 1;
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
