//
// Created by Yujin on 2024/02/22.
//

#include "UI.h"

#include "OverlayTitle.h"

void UI::notify(std::string widgetName, int widgetAction)
{
    if (widgetName == "start" && widgetAction == 0)
    {
        //UI::Instance().changeOverlay(std::make_unique<OverlayScene>());
    }
}

void UI::loadFiles(VkPhysicalDevice newPhysicalDevice, VkDevice newLogicalDevice, VkQueue transferQueue, VkCommandPool transferCommandPool, VkDescriptorPool samplerDescriptorPool, VkDescriptorSetLayout samplerSetLayout)
{
    for (auto it : fileNames)
    {
        add(it, newPhysicalDevice, newLogicalDevice, transferQueue, transferCommandPool, samplerDescriptorPool, samplerSetLayout,
            0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 1.f, 1.f);
    }
}

void UI::add(std::string name, VkPhysicalDevice newPhysicalDevice, VkDevice newLogicalDevice, VkQueue transferQueue, VkCommandPool transferCommandPool, VkDescriptorPool samplerDescriptorPool, VkDescriptorSetLayout samplerSetLayout,
             float positionX, float positionY, float positionZ, float scaleW, float scaleH, float angle, float r, float g, float b, float a)
{
    std::shared_ptr<Sprite> sprite = std::make_shared<Sprite>(name);
    sprite->setSpriteValues(positionX, positionY, positionZ, scaleW, scaleH, angle, r, g, b, a);
    sprites.emplace_back(sprite);
    sprite->loadFile(newPhysicalDevice, newLogicalDevice, transferQueue, transferCommandPool, samplerDescriptorPool, samplerSetLayout);
}

void UI::update(HighResolutionTimer timer, float elapsedTime)
{

}

void UI::render(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout)
{
    if (currentOverlay != nullptr)
    {
        currentOverlay->render(commandBuffer, pipelineLayout);
    }
}

void UI::clear()
{
    if (currentOverlay != nullptr)
    {
        currentOverlay->finalize();
        currentOverlay.release();
    }
}

void UI::cleanup(VkDevice newLogicalDevice)
{
    for (auto it : sprites)
    {
        it->cleanupResourses(newLogicalDevice);
    }
}

void UI::changeOverlay(std::unique_ptr<Overlay> overlay)
{
    nextOverlay = std::move(overlay);
}