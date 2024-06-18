#pragma once
#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h> 
#include <iostream>
using namespace std;

class Face {
    public:
        int vert_a;
        int vert_b;
        int vert_c;
        SDL_FPoint tex_vert;
        int norm_a;
        int norm_b;
        int norm_c;
        Face() {}
        Face(int a, int b, int c) {vert_a=a;vert_b=b;vert_c=c;}
};

std::ostream& operator << (std::ostream& out,Face const& a) {
    out << "1: "<<a.vert_a<<" 2: "<<a.vert_b<< " 3: "<<a.vert_c;
    return out;
}
