#include "index_buffer.hpp"

IndexBuffer::IndexBuffer(void* data, int numIndices, uint8_t elementSize) {
    glGenBuffers(1, &bufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * elementSize, data, GL_DYNAMIC_DRAW);
}

IndexBuffer::~IndexBuffer() { glDeleteBuffers(1, &bufferID); }

void IndexBuffer::bind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID); }

void IndexBuffer::unbind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
