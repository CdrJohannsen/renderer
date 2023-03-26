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

float getCos(int degree) {
    return cos((M_PI/180)*degree);
}

float getSin(int degree) {
    return sin((M_PI/180)*degree);
}

tuple<float,float> convert3Dto2D(x,y,z){
    return make_tuple(x2,y2)
}

vector<SDL_Vertex> faceToVerts(Face face){
    cout<<face<<endl;
    tie(x,y) = convert3Dto2D(face.vert_a.x,face.vert_a.y,face.vert_a.z);
    vector<SDL_Vertex> verts= {
        {
            SDL_FPoint{face.vert_a.x,face.vert_a.y,face.vert_a.z},
            SDL_Color{255,255,255,255},
            SDL_FPoint{0}
        },
        {
            SDL_FPoint{face.vert_b.x,face.vert_b.y,face.vert_b.z},
            SDL_Color{255,255,0,255},
            SDL_FPoint{0}
        },
        {
            SDL_FPoint{face.vert_c.x,face.vert_c.y,face.vert_c.z},
            SDL_Color{255,0,255,255},
            SDL_FPoint{0}
        },
    };
    return verts;
}

void drawFace(SDL_Renderer* renderer,Face &face){
    vector<SDL_Vertex> verts = faceToVerts(face);
    SDL_RenderGeometry(renderer,nullptr,verts.data(),verts.size(),nullptr,0);
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
    
    vector<Face> faces=parse();
    for (Face &face:faces){
        drawFace(renderer,face);
    }

    return 0;
}

