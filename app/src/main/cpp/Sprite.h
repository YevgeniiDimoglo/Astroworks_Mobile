//
// Created by Yujin on 2024/02/21.
//

#ifndef ASTROWORKS_MOBILE_SPRITE_H
#define ASTROWORKS_MOBILE_SPRITE_H

#include <android/asset_manager.h>

#include "Utilities.h"
#include "Texture.h"

class Sprite
{
public:

    std::string filePath;
    std::string fileName;
    std::string fileType;

    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 uv;
        glm::vec4 tangent;

        static VkVertexInputBindingDescription getBindingDescription()
        {
            VkVertexInputBindingDescription bingingDescription{};
            bingingDescription.binding = 0;
            bingingDescription.stride = sizeof(Vertex);
            bingingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bingingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions()
        {
            std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};
            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, pos);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, color);

            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[2].offset = offsetof(Vertex, uv);

            attributeDescriptions[3].binding = 0;
            attributeDescriptions[3].location = 2;
            attributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[3].offset = offsetof(Vertex, tangent);

            return attributeDescriptions;
        }

        bool operator==(const Vertex& other) const {
            return pos == other.pos && color == other.color && uv == other.uv;
        }
    };

    glm::mat4 spriteValues = glm::mat4(1.0f);

    Sprite() {};
    Sprite(std::string filePath);

    void setSpriteValues(float positionX, float positionY, float positionZ, float width, float height, float angle, float r, float g, float b, float a)
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

    float getPositionX() const { return positionX; }
    float getPositionY() const { return positionY; }
    float getPositionZ() const { return positionZ; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }
    float getAngle() const { return angle; }
    float getRColor() const { return r; }
    float getBColor() const { return g; }
    float getGColor() const { return b; }
    float getAColor() const { return a; }

    void loadFile(AAssetManager& assetManager, VkPhysicalDevice newPhysicalDevice, VkDevice newLogicalDevice, VkQueue transferQueue, VkCommandPool transferCommandPool, VkDescriptorPool samplerDescriptorPool, VkDescriptorSetLayout samplerSetLayout);
    void draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout);

    void updateTransform();

    void cleanupResourses(VkDevice newLogicalDevice);

private:

    struct Image
    {
        VkImage					image;
        VkImageLayout			imageLayout;
        VkDeviceMemory			deviceMemory;
        VkImageView				view;
        uint32_t				width, height;
        VkDescriptorImageInfo	descriptor;
        VkSampler				sampler;
        VkDescriptorSet			descriptorSet;
    };

    struct
    {
        int				count;
        VkBuffer		buffer;
        VkDeviceMemory	memory;
    } vertices;

    struct
    {
        int				count;
        VkBuffer		buffer;
        VkDeviceMemory	memory;
    } indices;

private:
    Image image;

    VkPhysicalDevice newPhysicalDevice;
    VkDevice newLogicalDevice;
    VkQueue transferQueue;
    VkCommandPool commandPool;
    VkDescriptorPool samplerDescriptorPool;
    VkDescriptorSetLayout samplerSetLayout;

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

    void createVertexBuffer(VkPhysicalDevice newPhysicalDevice, VkDevice newLogicalDevice, VkQueue transferQueue, VkCommandPool transferCommandPool, std::vector<Vertex>* vertices);
    void createIndexBuffer(VkPhysicalDevice newPhysicalDevice, VkDevice newLogicalDevice, VkQueue transferQueue, VkCommandPool transferCommandPool, std::vector<uint32_t>* indices);
    Image createTextureFromBuffer(void* buffer, VkDeviceSize bufferSize, VkFormat format, uint32_t texWidth, uint32_t texHeight, VkPhysicalDevice newPhysicalDevice, VkDevice newLogicalDevice, VkCommandPool transferCommandPool, VkQueue transferQueue);
};


#endif //ASTROWORKS_MOBILE_SPRITE_H
