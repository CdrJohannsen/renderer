#pragma once
#include <GL/glew.h>
#include <SDL2/SDL.h>

struct FrameBuffer {
    void create(uint32_t width, uint32_t heigth);

    GLuint getTextureID(int i);

    void destroy();

    void bind();

    void unbind();

   private:
    GLuint fbo;
    GLuint textures[2];
};
