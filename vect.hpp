#pragma once
#include <math.h>
#include <stdio.h> 
#include <iostream>
using namespace std;

class Vect {
    public:
        float x;
        float y;
        float z;
        float w = 1;
        Vect operator + (Vect a){return Vect {x+a.x,y+a.y,z+a.z};}
        Vect operator - (Vect a){return Vect {x-a.x,y-a.y,z-a.z};}
        Vect operator - (float a){return Vect {x-a,y-a,z-a};}
        Vect operator * (Vect a){return Vect {x*a.x,y*a.y,z*a.z};}
        Vect operator * (float a){return Vect {x*a,y*a,z*a};}
        Vect operator / (Vect a){return Vect {x/a.x,y/a.y,z/a.z};}
        Vect operator / (float a){return Vect {x/a,y/a,z/a};}
        void operator += (Vect a) { x += a.x;y+=a.y; }
        void operator -= (Vect a) { x -= a.x;y-=a.y; }
        void operator *= (Vect a) { x *= a.x;y*=a.y; }
        void operator *= (float a) { x *= a;y*=a; }
        void operator /= (Vect a) { x /= a.x;y/=a.y; }
        Vect(float a,float b,float c) {x=a;y=b;z=c;}
        Vect() {}
        //Vect(float a[]) {x=a[0];y=a[1];z=a[2];}
        float length() {
            return sqrt(pow(x,2)+pow(y,2)+pow(z,2));
        }
        Vect normalized(){
            return Vect(x,y,z)/this->length();
        }
};

std::ostream& operator << (std::ostream& out,Vect const& a) {
    out << "x: "<<a.x<<" y: "<<a.y<< " z: "<<a.z;
    return out;
}
