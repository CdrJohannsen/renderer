#pragma once
#include <GL/glew.h>
#include "shader.hpp"
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include <stb/stb_truetype.h>

struct FontVertex {
    glm::vec2 position;
    glm::vec2 texCoord;
};

struct Font {
    ~Font() ;

    void initFont(char* filename) ;

    void drawString(float x, float y, const char* text, Shader* fontShader) ;

   private:
    stbtt_bakedchar cdata[96];
    GLuint fontTexture;
    GLuint fontVao;
    GLuint fontVertexBufferID;
    FontVertex* fontVertexBufferData = 0;
    uint32_t fontVertexBufferCapacity;
};
