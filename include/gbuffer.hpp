#pragma once
#include <GL/glew.h>

struct GBuffer {
    void create(uint32_t w, uint32_t h);

    void destroy();

    void bind();

    void unbind();

    void bindTexture();

    void blitFramebuffer();

   private:
    GLuint fbo;
    GLuint gPosition;
    GLuint gNormal;
    GLuint gColorSpec;
    GLuint gEmissive;
    uint32_t width;
    uint32_t heigth;
};
