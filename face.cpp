#include <math.h>
#include <stdio.h> 
#include <iostream>
using namespace std;

class Face {
    public:
        Vect vert_a=Vect(0,0,0);
        Vect vert_b=Vect(0,0,0);
        Vect vert_c=Vect(0,0,0);
        SDL_FPoint tex_vert;
        float norm_a;
        float norm_b;
        float norm_c;
        Face() {}
        Face(Vect a, Vect b, Vect c) {vert_a=a;vert_b=b;vert_c=c;}
};

std::ostream& operator << (std::ostream& out,Face const& a) {
    out << "1: "<<a.vert_a<<" 2: "<<a.vert_b<< " 3: "<<a.vert_c;
    return out;
}
