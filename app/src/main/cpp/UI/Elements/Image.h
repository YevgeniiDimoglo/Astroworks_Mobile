//
// Created by Yujin on 2024/02/21.
//

#ifndef ASTROWORKS_MOBILE_IMAGE_H
#define ASTROWORKS_MOBILE_IMAGE_H

#include "../../Sprite.h"
#include "../UIUtilites.h"

class Image : public Widget
{
public:
    Image() {}
    Image(std::string name);

    void setImageValues(float positionX, float positionY, float positionZ, float width, float height, float angle, float r, float g, float b, float a) override;

    void updateSpriteValues() override;

    void changeValue(float& value) override;

    void updateValues(float timer, int value) override;

    void draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout) override;

    void update(float elapsedTime) override;

private:

    float positionX;
    float positionY;
    float positionZ;
    float width;
    float height;
    float angle;
    float r;
    float g;
    float b;
    float a;

    std::shared_ptr<Sprite> image;
};

#endif //ASTROWORKS_MOBILE_IMAGE_H
