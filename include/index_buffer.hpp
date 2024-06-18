#pragma once
#include <GL/glew.h>

struct IndexBuffer {
    IndexBuffer(void* data,int numIndices,uint8_t elementSize){
        glGenBuffers(1, &bufferID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,bufferID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,numIndices*elementSize,data,GL_DYNAMIC_DRAW);
    }

    virtual ~IndexBuffer(){
        glDeleteBuffers(1,&bufferID);
    }

    void bind(){
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,bufferID);
    }

    void unbind(){
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    }

private:
    GLuint bufferID;
};
