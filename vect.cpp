#include <math.h>
#include <stdio.h> 
#include <iostream>
using namespace std;

class Vect {
    public:
        float x;
        float y;
        float z;
        Vect operator + (Vect a){return Vect {x+a.x,y+a.y};}
        Vect operator - (Vect a){return Vect {x-a.x,y-a.y};}
        Vect operator * (Vect a){return Vect {x*a.x,y*a.y};}
        Vect operator * (float a){return Vect {x*a,y*a};}
        Vect operator / (Vect a){return Vect {x/a.x,y/a.y};}
        Vect operator / (float a){return Vect {x/a,y/a};}
        void operator += (Vect a) { x += a.x;y+=a.y; }
        void operator -= (Vect a) { x -= a.x;y-=a.y; }
        void operator *= (Vect a) { x *= a.x;y*=a.y; }
        void operator *= (float a) { x *= a;y*=a; }
        void operator /= (Vect a) { x /= a.x;y/=a.y; }
        Vect(float a,float b) {x=a;y=b;}
        //Vect(float a[]) {x=a[0];y=a[1];z=a[2];}
        Vect(string a[]) {}
        void reverse() {
            float a=x;
            x=y;
            y=a;
        }
        float length() {
            return sqrt(pow(x,2)+pow(y,2));
        }
};

std::ostream& operator << (std::ostream& out,Vect const& a) {
    out << "x: "<<a.x<<" y: "<<a.y<<"\n";
    return out;
}
