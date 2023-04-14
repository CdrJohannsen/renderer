#include <math.h>
#include <stdio.h> 
#include <iostream>
using namespace std;

class Vert {
    public:
        float x;
        float y;
        float z;
        
        float u;
        float v;

        float r;
        float g;
        float b;
        float a;
        Vert operator + (Vert a){return Vert {x+a.x,y+a.y,z+a.z};}
        Vert operator - (Vert a){return Vert {x-a.x,y-a.y,z-a.z};}
        Vert operator - (float a){return Vert {x-a,y-a,z-a};}
        Vert operator * (Vert a){return Vert {x*a.x,y*a.y,z*a.z};}
        Vert operator * (float a){return Vert {x*a,y*a,z*a};}
        Vert operator / (Vert a){return Vert {x/a.x,y/a.y,z/a.z};}
        Vert operator / (float a){return Vert {x/a,y/a,z/a};}
        void operator += (Vert a) { x += a.x;y+=a.y; }
        void operator -= (Vert a) { x -= a.x;y-=a.y; }
        void operator *= (Vert a) { x *= a.x;y*=a.y; }
        void operator *= (float a) { x *= a;y*=a; }
        void operator /= (Vert a) { x /= a.x;y/=a.y; }
        Vert(float a,float b,float c) {x=a;y=b;z=c;}
        Vert(Vect a) {x=a.x;y=a.y;z=a.z;}
        Vert() {}
        //Vert(float a[]) {x=a[0];y=a[1];z=a[2];}
        float length() {
            return sqrt(pow(x,2)+pow(y,2)+pow(z,2));
        }
        Vert normalized(){
            return Vert(x,y,z)/this->length();
        }
        void setRGBA(Vect v){r=v.x;g=v.y;b=v.z;a=v.w;}
};

std::ostream& operator << (std::ostream& out,Vert const& a) {
    out << "x: "<<a.x<<" y: "<<a.y<< " z: "<<a.z;
    return out;
}
