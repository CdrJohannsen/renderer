#include <cstdio>

struct FontVertex {
    glm::vec2 position;
    glm::vec2 texCoord;
};

struct Font {
    void initFont(char* filename) {
        uint8_t ttfBuffer[1<<20];
        uint8_t tmpBitmap[512*512];

        fread(ttfBuffer, 1, 1<<20, fopen(filename,"rb"));
        stbtt_BakeFontBitmap(ttfBuffer, 0, 32.0f, tmpBitmap, 512, 512, 32, 96, cdata);
        
        glGenTextures(1, &fontTexture);
        glBindTexture(GL_TEXTURE_2D, fontTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, tmpBitmap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        glGenVertexArrays(1, &fontVao);
        glBindVertexArray(fontVao);
        glGenBuffers(1, &fontVertexBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, fontVertexBufferID);
        fontVertexBufferCapacity = 100;
        fontVertexBufferData = new FontVertex[fontVertexBufferCapacity * 6];
        glBufferData(GL_ARRAY_BUFFER, sizeof(FontVertex) * 6 * fontVertexBufferCapacity, 0,GL_DYNAMIC_DRAW);

    }

    void drawString(float x, float y,const char* text, Shader* fontShader ){

    }

private:
    stbtt_bakedchar cdata[96];
    GLuint fontTexture;
    GLuint fontVao;
    GLuint fontVertexBufferID;
    FontVertex* fontVertexBufferData = 0;
    uint32_t fontVertexBufferCapacity;
};
