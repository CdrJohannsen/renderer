#include <SDL2/SDL.h>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <tuple>
#include "vect.cpp"
#include "face.cpp"
#include "parseOBJ.cpp"
#include <iostream>
using namespace std;

#define FOCAL_LENGTH 4
#define WIDTH 1920
#define HEIGTH 1080
#define SCALE 50

float getCos(int degree) {
    return cos((M_PI/180)*degree);
}

float getSin(int degree) {
    return sin((M_PI/180)*degree);
}

tuple<float,float> convert3Dto2D(float x,float y,float z){
    float x2=(x/z)*FOCAL_LENGTH*SCALE;
    float y2=(y/z)*FOCAL_LENGTH*SCALE;
    return make_tuple(x2,y2);
}

vector<SDL_Vertex> faceToVerts(Face face,SDL_Renderer** renderer){
    cout<<face<<endl;
    float x1, y1,x2,y2,x3,y3;
    tie(x1,y1) = convert3Dto2D(face.vert_a.x,face.vert_a.y,face.vert_a.z);
    tie(x2,y2) = convert3Dto2D(face.vert_b.x,face.vert_b.y,face.vert_b.z);
    tie(x3,y3) = convert3Dto2D(face.vert_c.x,face.vert_c.y,face.vert_c.z);
    x1=(-200);
    y1=(-200);
    x2=200;
    y2=200;
    x3=(-300);
    y3=300;
    cout << x1 << " " << y1 <<endl;
    cout << x2 << " " << y2 <<endl;
    cout << x3 << " " << y3 <<endl;
    vector<SDL_Vertex> verts={
        {
            SDL_FPoint{x1+960,y1+540},
            SDL_Color{255,255,255,255},
            SDL_FPoint{0}
        },
        {
            SDL_FPoint{x2+960,y2+540},
            SDL_Color{255,255,0,255},
            SDL_FPoint{0}
        },
        {
            SDL_FPoint{x2+960,y2+540},
            SDL_Color{255,0,255,255},
            SDL_FPoint{0}
        },
    };
    SDL_RenderGeometry(*renderer,nullptr,verts.data(),verts.size(),nullptr,0);
    SDL_RenderPresent(*renderer);
    return verts;
}

void drawFace(SDL_Renderer** renderer,Face &face){
    vector<SDL_Vertex> verts = faceToVerts(face,renderer);
    cout << verts[0].position.x<<endl;
    SDL_RenderGeometry(*renderer,nullptr,verts.data(),verts.size(),nullptr,0);
}

int main()
{
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Event e;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(1920,1080,SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL,&window, &renderer);

    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer,255,255,255,255);
    
    vector<Face> faces=parse();
    for (Face &face:faces){
        drawFace(&renderer,face);
    }

    //SDL_RenderPresent(renderer);
    SDL_Delay(10000);

    return 0;
}

