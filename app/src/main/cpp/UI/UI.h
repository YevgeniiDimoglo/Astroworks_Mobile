//
// Created by Yujin on 2024/02/21.
//

#ifndef ASTROWORKS_MOBILE_UI_H
#define ASTROWORKS_MOBILE_UI_H

#include "../Utilities.h"
#include "../Sprite.h"

#include "UIUtilites.h"

class UI : public IObserver
{
private:
    UI() {}
    virtual ~UI() {}

public:
    static UI& Instance()
    {
        static UI instance;
        return instance;
    }

    void notify(std::string widgetName, int widgetAction) override;

    void setFileNames(std::vector<std::string> filePathes) { fileNames = filePathes; }

    void setFileNamesAndroid(std::vector<std::string> filePathes) { fileNames = filePathes; }

    std::vector<std::shared_ptr<Sprite>> getSprites() const { return sprites; }

    Overlay& getCurrentOverlay() const { return *this->currentOverlay; }

    void loadFiles(AAssetManager *assetManager, VkPhysicalDevice newPhysicalDevice, VkDevice newLogicalDevice, VkQueue transferQueue, VkCommandPool transferCommandPool, VkDescriptorPool samplerDescriptorPool, VkDescriptorSetLayout samplerSetLayout);

    void add(std::string name, AAssetManager *assetManager, VkPhysicalDevice newPhysicalDevice, VkDevice newLogicalDevice, VkQueue transferQueue, VkCommandPool transferCommandPool, VkDescriptorPool samplerDescriptorPool, VkDescriptorSetLayout samplerSetLayout,
             float positionX, float positionY, float positionZ, float scaleW, float scaleH, float angle, float r, float g, float b, float a);

    void update(HighResolutionTimer timer, float elapsedTime);

    void render(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout);

    void clear();

    void cleanup(VkDevice newLogicalDevice);

    void changeOverlay(std::unique_ptr<Overlay> overlay);

private:

    enum class  OverlayUUIDs
    {
        OverlayTitle,
        OverlayGame,
        OverlayLoading,
        OverlayFinish,

        EnumCount
    };

    std::unique_ptr<Overlay> currentOverlay;
    std::unique_ptr<Overlay> nextOverlay;

    std::vector<std::string> fileNames;
    std::vector<std::shared_ptr<Sprite>> sprites;
};

#endif //ASTROWORKS_MOBILE_UI_H
