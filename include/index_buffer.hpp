#pragma once
#include <GL/glew.h>

struct IndexBuffer {
    IndexBuffer(void* data, int numIndices, uint8_t elementSize);

    virtual ~IndexBuffer();

    void bind();

    void unbind();

   private:
    GLuint bufferID;
};
