#pragma once
#include <math.h>
#include <stdio.h> 
#include <iostream>
using namespace std;

struct Matrix4x4 {
    public:
        float m[4][4] { 0.0f };
};

std::ostream& operator << (std::ostream& out,Matrix4x4 const& a) {
    out << "x: ";
    return out;
}
