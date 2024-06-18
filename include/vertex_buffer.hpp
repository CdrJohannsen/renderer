#pragma once
#include <GL/glew.h>

struct VertexBuffer {
    VertexBuffer(void* data, int numVerticies);

    virtual ~VertexBuffer();

    void bind();

    void unbind();

   private:
    GLuint bufferID;
    GLuint vao;
};
