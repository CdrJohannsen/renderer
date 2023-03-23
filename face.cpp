#include <math.h>
#include <stdio.h> 
#include <iostream>
using namespace std;

class Face {
    public:
        int vert_a;
        int vert_b;
        int vert_c;
        int tex_a;
        int tex_b;
        int tex_c;
        int norm_a;
        int norm_b;
        int norm_c;

        Face(int x,int y,int z) {vert_a=x;vert_b=y;vert_c=z;}
};

std::ostream& operator << (std::ostream& out,Face const& a) {
    out << "1: "<<a.vert_a<<" 2: "<<a.vert_b<< " 3: "<<a.vert_c;
    return out;
}
