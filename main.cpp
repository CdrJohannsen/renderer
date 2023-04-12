
#include <SDL2/SDL.h>
#define GLEW_STATIC
#include <GL/glew.h>
#define SDL_MAIN_HANDLED
#include <GL/gl.h>
#include <SDL2/SDL_opengl.h>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include "vect.cpp"
#include "face.cpp"
#include "matr.h"
#include "parseOBJ.cpp"
#include <iostream>
using namespace std;

#define WIDTH 1920.0f
#define HEIGTH 1080.0f
#define RATIO 1920.0f/1080.0f
#define SCALE 50
#define NEAR 0.1f
#define FAR 1000.0f
#define FOV 90.0f
#define FOVRAD tan(FOV*0.5f/180.0f*M_PI)

static GLboolean should_rotate = GL_TRUE;

Matrix4x4 MatrixMultiply(Matrix4x4 &m1, Matrix4x4 &m2) {
    Matrix4x4 matrix;
    for (int c = 0; c < 4; c++)
        for (int r = 0; r < 4; r++)
            matrix.m[r][c] = m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] + m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];
    return matrix;
}

Matrix4x4 VectorToMatrix(Vect &v) {
    Matrix4x4 matrix;
    matrix.m[0][0] = 1.0f;
    matrix.m[1][1] = 1.0f;
    matrix.m[2][2] = 1.0f;
    matrix.m[3][3] = 1.0f;
    matrix.m[3][0] = v.x;
    matrix.m[3][1] = v.y;
    matrix.m[3][2] = v.z;
    matrix.m[3][3] = v.w;
    return matrix;
}



Vect cr={0,0,0};
Vect cp={0,0,10};

float getCos(float degree) {
    return cos((M_PI/180)*degree);
}

float getSin(float degree) {
    return sin((M_PI/180)*degree);
}
float dot(Vect a, Vect b){
    return a.x*b.x+a.y*b.y+a.z*b.z;
}

Vect angleToVect(Vect a){
    // takes 3 angles (filled into a Vector, but not actually a vector) and converts them to a real Vector
    Vect out;
    out.z=getCos(a.x)*getCos(a.y)*getCos(a.z);
    out.x=getCos(a.y)*getCos(a.z)*getCos(a.x);
    out.y=getCos(a.z)*getCos(a.x)*getCos(a.y);
    return out;
}


void renderTriangle(SDL_Renderer* renderer,vector<SDL_Vertex> v){
    SDL_SetRenderDrawColor(renderer,255,255,255,255);
    SDL_RenderDrawLine(renderer,v[0].position.x,v[0].position.y,v[1].position.x,v[1].position.y);
    SDL_RenderDrawLine(renderer,v[1].position.x,v[1].position.y,v[2].position.x,v[2].position.y);
    SDL_RenderDrawLine(renderer,v[2].position.x,v[2].position.y,v[0].position.x,v[0].position.y);
}

Matrix4x4 convert3Dto2D(Vect v){
    Vect diff=v-cp;
    Matrix4x4 m = VectorToMatrix(diff);
    /*Matrix4x4 projMat;
      projMat.m[0][0]=RATIO/FOVRAD;
      projMat.m[1][1]=FOVRAD;
      projMat.m[2][2]=FAR/(FAR-NEAR);
      projMat.m[3][2]=(-FAR*NEAR)/(FAR-NEAR);
      projMat.m[2][3]=1.0f;
      projMat.m[3][3]=0.0f;*/
    Matrix4x4 projMat;
    projMat.m[0][0]=1/(RATIO*FOVRAD);
    projMat.m[1][1]=1/FOVRAD;
    projMat.m[2][2]=FAR/(FAR-NEAR);
    projMat.m[2][3]=(-FAR*NEAR)/(FAR-NEAR);
    projMat.m[3][2]=1.0f;
    projMat.m[3][3]=0.0f;
    Matrix4x4 rotxMat;
    rotxMat.m[0][0]=1;
    rotxMat.m[1][1]=getCos(cr.x);
    rotxMat.m[1][2]=getSin(cr.x);
    rotxMat.m[2][1]=(-getSin(cr.x));
    rotxMat.m[2][2]=getCos(cr.x);
    rotxMat.m[3][3]=1;
    Matrix4x4 rotyMat;
    rotyMat.m[0][0]=getCos(cr.y);
    rotyMat.m[1][1]=1;
    rotyMat.m[2][2]=getCos(cr.y);
    rotyMat.m[0][2]=(-getSin(cr.y));
    rotyMat.m[2][0]=getSin(cr.y);
    rotyMat.m[3][3]=1;
    Matrix4x4 rotzMat;
    rotzMat.m[0][0]=getCos(cr.z);
    rotzMat.m[0][1]=getSin(cr.z);
    rotzMat.m[1][0]=(-getSin(cr.z));
    rotzMat.m[1][1]=getCos(cr.z);
    rotzMat.m[2][2]=1;
    rotzMat.m[3][3]=1;
    Matrix4x4 o1;
    Matrix4x4 o2;
    Matrix4x4 o3;
    Matrix4x4 o4;
    o1 = MatrixMultiply(m,rotyMat);
    o2 = MatrixMultiply(o1,rotxMat);
    o3 = MatrixMultiply(o2,rotzMat);
    //m = MatrixMultiply(m,projMat);
    /*o3.m[3][0]/=o3.m[3][2]*FOVRAD*RATIO;
      o3.m[3][1]/=o3.m[3][2]*FOVRAD;*/
    /*
       float dx=getCos(cr.y)*(getSin(cr.z)*diff.y-getCos(cr.z)*diff.x)-getSin(cr.y)*diff.z;
       float dy=getSin(cr.x)*(getCos(cr.y)*diff.z+getSin(cr.y)*(getSin(cr.z)*diff.y+getCos(cr.z)*diff.x)+getCos(cr.x)*(getCos(cr.z)*diff.y-getSin(cr.z)*diff.x));
       float dz=getCos(cr.x)*(getCos(cr.y)*diff.z+getSin(cr.y)*(getSin(cr.z)*diff.y-getCos(cr.z)*diff.x)-getSin(cr.x)*(getCos(cr.z)*diff.y-getSin(cr.z)*diff.x));

       Vect diff=c-v;
       float dx=getCos(cr.y)*(getSin(cr.z)*diff.y+getCos(cr.z)*diff.x)-getSin(cr.y)*diff.z;
       float dy=getSin(cr.x)*(getCos(cr.y)*diff.z+getSin(cr.y)*(getSin(cr.z)*diff.y+getCos(cr.z)*diff.x)+getCos(cr.x)*(getCos(cr.z)*diff.y-getSin(cr.z)*diff.x));
       float dz=getCos(cr.x)*(getCos(cr.y)*diff.z+getSin(cr.y)*(getSin(cr.z)*diff.y+getCos(cr.z)*diff.x)-getSin(cr.x)*(getCos(cr.z)*diff.y-getSin(cr.z)*diff.x));
       float x = (2/dz)*dx;
       float y = (2/dz)*dy;
       */

    return o3;
}

void drawFace(SDL_Renderer** renderer,Face &face){
    Vect to_point(verticies[face.vert_a].x,verticies[face.vert_a].y,verticies[face.vert_a].z);
    Vect to_point_rel = to_point-cp;
    float diff_dot = dot(normals[face.norm_a].normalized(),to_point_rel.normalized());
    if (diff_dot>0.0f){return;}
    Matrix4x4 a = convert3Dto2D(verticies[face.vert_a]);
    Matrix4x4 b = convert3Dto2D(verticies[face.vert_b]);
    Matrix4x4 c = convert3Dto2D(verticies[face.vert_c]);
    //to_point = angleToVect(cr);
    a.m[3][0]/=a.m[3][2]*FOVRAD*RATIO;
    a.m[3][1]/=a.m[3][2]*FOVRAD;
    b.m[3][0]/=b.m[3][2]*FOVRAD*RATIO;
    b.m[3][1]/=b.m[3][2]*FOVRAD;
    c.m[3][0]/=c.m[3][2]*FOVRAD*RATIO;
    c.m[3][1]/=c.m[3][2]*FOVRAD;
    float x1 = a.m[3][0]*WIDTH/2;
    float x2 = b.m[3][0]*WIDTH/2;
    float x3 = c.m[3][0]*WIDTH/2;
    float y1 = a.m[3][1]*HEIGTH/2;
    float y2 = b.m[3][1]*HEIGTH/2;
    float y3 = c.m[3][1]*HEIGTH/2;
    const vector<SDL_Vertex> verts={
        {
            SDL_FPoint{960+x1,540+y1},
            SDL_Color{127.0f*(normals[face.norm_a].x+1.0f),127.0f*(normals[face.norm_a].y+1.0f),127.0f*(normals[face.norm_a].z+1.0f),255},
            SDL_FPoint{0}
        },
        {
            SDL_FPoint{960+x2,540+y2},
            SDL_Color{127.0f*(normals[face.norm_b].x+1.0f),127.0f*(normals[face.norm_b].y+1.0f),127.0f*(normals[face.norm_b].z+1.0f),255},
            SDL_FPoint{0}
        },
        {
            SDL_FPoint{960+x3,540+y3},
            SDL_Color{127.0f*(normals[face.norm_c].x+1.0f),127.0f*(normals[face.norm_c].y+1.0f),127.0f*(normals[face.norm_c].z+1.0f),255},
            SDL_FPoint{0}
        },
    };
    SDL_RenderGeometry(*renderer,nullptr,verts.data(),verts.size(),nullptr,0);
    //renderTriangle(*renderer,verts);
    return;
}
bool handleInput(SDL_Event &e){
    while (SDL_PollEvent(&e)) {
        if(e.type == SDL_QUIT)
            return false;
        else if(e.type == SDL_KEYDOWN)
        {
            if(SDLK_w == e.key.keysym.sym) {
                cp.x += getSin(cr.y);
                cp.z -= getCos(cr.y);
            }
            else if(SDLK_s == e.key.keysym.sym) {
                cp.x -= getSin(cr.y);
                cp.z += getCos(cr.y);
            }
            else if(SDLK_a == e.key.keysym.sym) {
                cp.x += getCos(cr.y);
                cp.z += getSin(cr.y);
            }
            else if(SDLK_d == e.key.keysym.sym) {
                cp.x -= getCos(cr.y);
                cp.z -= getSin(cr.y);
            }
            else if(SDLK_SPACE == e.key.keysym.sym) {
                cp.y += 1;
            }
            else if(SDLK_LSHIFT == e.key.keysym.sym) {
                cp.y -= 1;
            }
            else if(SDLK_RIGHT == e.key.keysym.sym) {
                cr.y -= 1;
            }
            else if(SDLK_LEFT == e.key.keysym.sym) {
                cr.y += 1;
            }
            else if(SDLK_UP == e.key.keysym.sym) {
                cr.x -= 1;
            }
            else if(SDLK_DOWN == e.key.keysym.sym) {
                cr.x += 1;
            }
            else if(SDLK_RCTRL == e.key.keysym.sym) {
                cr.z += 1;
            }
            else if(SDLK_KP_0 == e.key.keysym.sym) {
                cr.z -= 1;
            }
            else if(SDLK_ESCAPE == e.key.keysym.sym) {
                return false;
            }
            break;
        }
    }
    return true;
}

int main()
{
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Event e;
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_BUFFER_SIZE, 32 );
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

    window = SDL_CreateWindow("Renderer", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,1920,1080, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    GLenum err = glewInit();
    if (err != GLEW_OK){
        cout << "Error: " << glewGetErrorString(err) << endl;
        cout << err << endl;
        cin.get();
    }


    //SDL_CreateWindowAndRenderer(1920,1080,SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL,&window, &renderer);
    SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" );
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);


    //SDL_SetRenderDrawColor(renderer,255,255,255,255);

    vector<Face> faces=parse();
    bool running=true;
    glClearColor(0.0f,0.0f,0.0f,1.0f);
    while(running) {
        glClear(GL_COLOR_BUFFER_BIT);
        running = handleInput(e);
        //SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_GL_SwapWindow(window);
        //SDL_RenderClear(renderer);
        for (Face &face:faces){
            //drawFace(&renderer,face);
        }

        //SDL_RenderPresent(renderer);
        SDL_Delay(10);

    }
    return 0;
}
