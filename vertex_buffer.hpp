#pragma once
#include <GL/glew.h>
#include "vert.hpp"

struct VertexBuffer {
    VertexBuffer(void* data,int numVerticies){
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &bufferID);
        glBindBuffer(GL_ARRAY_BUFFER,bufferID);
        glBufferData(GL_ARRAY_BUFFER,numVerticies*sizeof(Vert),data,GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), (void*) offsetof(struct Vert,position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), (void*) offsetof(struct Vert,normal));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), (void*) offsetof(struct Vert,tangent));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vert), (void*) offsetof(struct Vert,texCoord));

        glBindVertexArray(0);
    }

    virtual ~VertexBuffer(){
        glDeleteBuffers(1,&bufferID);
    }

    void bind(){
        glBindVertexArray(vao);
    }

    void unbind(){
        glBindVertexArray(0);
    }

private:
    GLuint bufferID;
    GLuint vao;
};
