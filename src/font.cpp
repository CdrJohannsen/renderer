#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <cstdio>
#include <glm/glm.hpp>

#include "font.hpp"

Font::~Font() {
    if (fontVertexBufferData) {
        delete[] fontVertexBufferData;
    }
}

void Font::initFont(char* filename) {
    uint8_t ttfBuffer[1 << 20];
    uint8_t tmpBitmap[512 * 512];

    fread(ttfBuffer, 1, 1 << 20, fopen(filename, "rb"));
    stbtt_BakeFontBitmap(ttfBuffer, 0, 16.0f, tmpBitmap, 512, 512, 32, 96, cdata);

    glGenTextures(1, &fontTexture);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, tmpBitmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenVertexArrays(1, &fontVao);
    glBindVertexArray(fontVao);
    glGenBuffers(1, &fontVertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, fontVertexBufferID);
    fontVertexBufferCapacity = 100;
    fontVertexBufferData = new FontVertex[fontVertexBufferCapacity * 6];
    glBufferData(GL_ARRAY_BUFFER, sizeof(FontVertex) * 6 * fontVertexBufferCapacity, 0, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(FontVertex), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FontVertex),
                          (const void*)offsetof(FontVertex, texCoord));
    glBindVertexArray(0);
}

void Font::drawString(float x, float y, const char* text, Shader* fontShader) {
    glBindVertexArray(fontVao);
    glBindBuffer(GL_ARRAY_BUFFER, fontVertexBufferID);

    uint32_t len = strlen(text);
    if (fontVertexBufferCapacity < len) {
        fontVertexBufferCapacity = len;
        glBufferData(GL_ARRAY_BUFFER, sizeof(FontVertex) * 6 * fontVertexBufferCapacity, 0, GL_DYNAMIC_DRAW);
        delete[] fontVertexBufferData;
        fontVertexBufferData = new FontVertex[fontVertexBufferCapacity * 6];
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    glUniform1i(glGetUniformLocation(fontShader->getShaderID(), "u_texture"), 0);

    FontVertex* vData = fontVertexBufferData;
    uint32_t numVertices = 0;
    while (*text) {
        if (*text >= 32 && *text < 128) {
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(cdata, 512, 512, *text - 32, &x, &y, &q, 1);

            vData[0].position = glm::vec2(q.x0, q.y0);
            vData[0].texCoord = glm::vec2(q.s0, q.t0);
            vData[1].position = glm::vec2(q.x1, q.y0);
            vData[1].texCoord = glm::vec2(q.s1, q.t0);
            vData[2].position = glm::vec2(q.x1, q.y1);
            vData[2].texCoord = glm::vec2(q.s1, q.t1);
            vData[3].position = glm::vec2(q.x0, q.y1);
            vData[3].texCoord = glm::vec2(q.s0, q.t1);
            vData[4].position = glm::vec2(q.x0, q.y0);
            vData[4].texCoord = glm::vec2(q.s0, q.t0);
            vData[5].position = glm::vec2(q.x1, q.y1);
            vData[5].texCoord = glm::vec2(q.s1, q.t1);

            vData += 6;
            numVertices += 6;
        }
        ++text;
    }
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(FontVertex) * numVertices, fontVertexBufferData);
    glDrawArrays(GL_TRIANGLES, 0, numVertices);
}
