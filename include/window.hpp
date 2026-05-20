#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "floating_camera.hpp"
#include "popup.hpp"
#include "window_surface.hpp"

class Window : public WindowSurface {
   public:
    Window(struct xdg_toplevel *toplevel);

    void render(FloatingCamera *camera) override;

    void motion(glm::vec3 pos, glm::vec3 rot) override;

    double getDistance(glm::vec3 pos, glm::vec3 rot) override;

    void close() override;

    void configure() override;

    void addPopup(Popup *popup) {
        popup->init(windowShader_);
        popups_.push_back(popup);
    }

    void updateModelMat() override;

#ifdef _DEBUG
    void renderDebugUI() override;
#endif

   private:
    struct xdg_toplevel *toplevel_ = nullptr;

    std::vector<Popup *> popups_;
};
