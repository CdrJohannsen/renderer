#include <math.h>
#include <stdio.h> 
#include <iostream>
using namespace std;

class Face {
    public:
        Vect* vert_a;
        Vect* vert_b;
        Vect* vert_c;
        SDL_FPoint tex_vert;
        float* norm_a;
        float* norm_b;
        float* norm_c;
        //Face(Vect* x,Vect* y,Vect* z) {vert_a=x;vert_b=y;vert_c=z;}
};

std::ostream& operator << (std::ostream& out,Face const& a) {
    out << "1: "<<*a.vert_a<<" 2: "<<*a.vert_b<< " 3: "<<*a.vert_c;
    return out;
}
