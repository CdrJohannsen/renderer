#pragma once

#include <glm/glm.hpp>
#include "window_surface.hpp"

class Popup : public WindowSurface {
   public:
    Popup(struct xdg_popup *popup);

    void close() override;

    void configure() override;

    void setOffset(int32_t x, int32_t y) {
        offset_.x = x / kSurfaceScaling;
        offset_.y = y / kSurfaceScaling;
        updateModelMat();
    }

    void updateModelMat() override;

    void setParentMat(glm::mat4 parentMat) { parentMat_ = parentMat; }

    void setParentSize(glm::vec2 parentSize) {
        parentSize_.x = parentSize.x / kSurfaceScaling;
        parentSize_.y = parentSize.y / kSurfaceScaling;
    }

   private:
    struct xdg_popup *popup_ = nullptr;

    glm::vec3 offset_ = {0, 0, 0.01};
    glm::vec2 parentSize_;

    glm::mat4 parentMat_;
};
