#pragma once

#include <glm/glm.hpp>
#include "floating_camera.hpp"
#include "shader.hpp"
#include "surface.hpp"

class WindowSurface : public Surface {
   public:
    WindowSurface(struct xdg_surface *surface);
    virtual ~WindowSurface();

    void init(Shader *windowShader) override;

    void render(FloatingCamera *camera) override;

    void commit() override;

    void setTitle(string title) { title_ = title; }
    void setAppID(string id) { appId_ = id; }
    void setMinSize(glm::vec2 minSize) { minSize_ = minSize; }
    void setMaxSize(glm::vec2 maxSize) { maxSize_ = maxSize; }

    virtual void motion(glm::vec3 pos, glm::vec3 rot);

    virtual double getDistance(glm::vec3 pos, glm::vec3 rot);

    void enter(glm::vec3 pos, glm::vec3 rot);

    void leave();

    void rotate(glm::vec3 pos, glm::vec3 rot);

    void move(glm::vec3 mov);

    void resize(glm::vec2 diff);

    void setPosition(glm::vec3 pos);

    void configure() override;

    void setSize(int32_t width, int32_t height) { size = {width, height}; }

    virtual void updateModelMat() = 0;

#ifdef _DEBUG
    virtual void renderDebugUI();
#endif

   protected:
    struct xdg_surface *xdg_surface_ = nullptr;

    string title_ = "unknown";
    string appId_ = "unknown";

    glm::vec3 position{0, 0, 3};
    glm::vec3 rotation{0, 0, 1};
    glm::vec2 size{200, 200};

    glm::vec2 minSize_{0, 0};
    glm::vec2 maxSize_{0, 0};

    glm::mat4 modelMat;
    glm::mat4 modelView;
    glm::mat4 modelViewProj;

    Shader *windowShader_;

    GLuint modelViewLocation;
    GLuint modelViewProjLocation;
    GLuint colorLocation;

    GLuint window_texture;

    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint IBO;
};
