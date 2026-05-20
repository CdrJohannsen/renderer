
#include "overlay.hpp"
#include "surface.hpp"

void Overlay::init() { loadOverlayVAO(); }

void Overlay::render(GLuint texture) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LEQUAL);
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
}

void Overlay::loadOverlayVAO() {
    glm::vec2 s = size_ / glm::vec2({1080 / 3, 1920 / 3});
    // clang-format off
    float overlayVertices[] = {
        -s.x/2.f,   s.y/2.f,
        s.x/2.f,    -s.y/2.f,
        s.x/2.f,    s.y/2.f,
        -s.x/2.f,   -s.y/2.f,
        s.x/2.f,    -s.y/2.f,
        -s.x/2.f,   s.y/2.f,
    };
    // clang-format on
    GLuint vbo;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &vbo);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(overlayVertices), &overlayVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
}
