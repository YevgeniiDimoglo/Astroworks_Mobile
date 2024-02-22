//
// Created by Yujin on 2024/02/21.
//

#ifndef ASTROWORKS_MOBILE_OVERLAYTITLE_H
#define ASTROWORKS_MOBILE_OVERLAYTITLE_H

#include "../Utilities.h"

#include "UIUtilites.h"

class OverlayTitle : public Overlay
{
public:
    OverlayTitle() {}
    ~OverlayTitle() override {}

    void initialize() override;

    void finalize() override;

    void update(float elapsedTime) override;

    void render(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout) override;
};

#endif //ASTROWORKS_MOBILE_OVERLAYTITLE_H
