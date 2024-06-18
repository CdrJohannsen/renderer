#pragma once
#include <GL/glew.h>

#include <string>
#include <vector>
using namespace std;

unsigned int loadCubemap(vector<string> faces);

vector<string> getCubemapTextures();

GLuint getSkyboxVAO();
