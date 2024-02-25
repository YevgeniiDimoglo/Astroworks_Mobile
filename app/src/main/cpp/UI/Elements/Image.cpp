//
// Created by Yujin on 2024/02/26.
//

#include "Image.h"

#include "../UI.h"

Image::Image(std::string name)
{
    auto sprites = UI::Instance().getSprites();
    for (auto it : sprites)
    {
        if (it->fileName == name)
        {
            image = it;
        }
    }

    widgetUniqueName = name;
}

void Image::setImageValues(float positionX, float positionY, float positionZ, float width, float height, float angle, float r, float g, float b, float a)
{
    this->positionX = positionX;
    this->positionY = positionY;
    this->positionZ = positionZ;
    this->width = width;
    this->height = height;
    this->angle = angle;
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

void Image::updateSpriteValues()
{
    image->setSpriteValues(positionX, positionY, positionZ, width, height, angle, r, g, b, a);
}

void Image::changeValue(float& value)
{
}

void Image::updateValues(float timer, int value)
{
}

void Image::draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout)
{
    updateSpriteValues();
    image->updateTransform();
    image->draw(commandBuffer, pipelineLayout);
}

void Image::update(float elapsedTime)
{
}