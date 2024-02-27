//
// Created by Yujin on 2024/02/10.
//

#ifndef HELLO_VULKAN_UTILITIES_H
#define HELLO_VULKAN_UTILITIES_H

// STD libs
#include <filesystem>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <cstring>
#include <string>
#include <cstdlib>
#include <cstdint>
#include <utility>
#include <limits>
#include <optional>
#include <vector>
#include <array>
#include <set>
#include <map>
#include <list>
#include <unordered_map>

// Vulkan libs
#include <assert.h>
#include <vulkan/vulkan.h>

// GLM libs
#include <GLES2/gl2.h>
#include "glm/glm.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_access.hpp"
#include "glm/gtc/type_ptr.hpp"

// Loader libs
#include "external/json.hpp"
#include "external/toml.hpp"

#include "HighResolutionTimer.h"

#include "external/stb_image.h"

const int MAX_FRAMES_IN_FLIGHT = 2;

static const int MAX_OBJECTS = 512;

static const uint32_t PARTICLE_COUNT = 8192;

static const uint32_t WIDTH = 1920;
static const uint32_t HEIGHT = 1080;

static const int syncInterval = 0;

static std::filesystem::path pathToProgram;

enum class Pipelines
{
    //ShadowMapPipeline,

    DebugDrawingPipeline,

    //UnlitPipeline,
    //PhongPipeline,
    //PBRPipeline,
    //PBRIBLPipeline,

    UIPipeline,

    //OffscreenPipeline,
    //OffscreenPipeline2,

    //OITColorAccumPipeline,
    //OITColorRevealPipeline,
    //OITResultPipeline,
    //DemoOITColorAccumPipeline,
    //DemoOITColorRevealPipeline,
    //DemoOITResultPipeline,

    //SkyboxPipeline,

    //LuminancePipeline,
    //BlurPipeline,

    //WaterPipeline,
    //FirePipeline,

    //ComputeParticlePipeline,

    EnumCount
};

enum class ShaderType
{
    Unlit,
    Flat,
    Phong,
    PBR,
    PBRIBL,

    OITColorAccum,
    OITColorReveal,
    OITResult,

    Skybox,

    Sprite,

    Water,
    Fireball,

    EnumCount
};

enum class TextureType
{
    Albedo = 0,
    Normal,
    Metalness,
    Roughness,
    AmbientOcclussion,
    Emissive,

    LUT,

    GlobalTexture1,
    NoiseTexture1,
    NoiseTexture2,

    EnumCount
};

struct ImageBuffer
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

struct FrameBufferAttachment {
    VkImage image;
    VkDeviceMemory mem;
    VkImageView view;
};

struct Framebuffer
{
    int32_t width, height;
    FrameBufferAttachment offscreenColorAttachment, offscreenDepthAttachment;
    VkSampler sampler;
    VkDescriptorImageInfo descriptor;
    VkDescriptorSet			descriptorSet;
};

struct PushConstants {
    glm::mat4 model;
    glm::vec4 baseColor;
    glm::vec4 timer;
    glm::vec4 additionalValues;
};

struct UniformBufferObject {
    std::array<float, 16> mvp;
};

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

#define LOG_TAG "hellovkjni"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define VK_CHECK(x)                           \
  do {                                        \
    VkResult err = x;                         \
    if (err) {                                \
      LOGE("Detected Vulkan error: %d", err); \
      abort();                                \
    }                                         \
  } while (0)

static const char *toStringMessageSeverity(VkDebugUtilsMessageSeverityFlagBitsEXT s) {
    switch (s) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            return "VERBOSE";
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            return "ERROR";
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            return "WARNING";
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            return "INFO";
        default:
            return "UNKNOWN";
    }
}

static const char *toStringMessageType(VkDebugUtilsMessageTypeFlagsEXT s) {
    if (s == (VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
              VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
              VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT))
        return "General | Validation | Performance";
    if (s == (VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
              VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT))
        return "Validation | Performance";
    if (s == (VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
              VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT))
        return "General | Performance";
    if (s == (VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT))
        return "Performance";
    if (s == (VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
              VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT))
        return "General | Validation";
    if (s == VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) return "Validation";
    if (s == VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) return "General";
    return "Unknown";
}

class IObserver {
public:
    virtual ~IObserver() {};
    virtual void notify(std::string name, int action) = 0;
};

#endif //HELLO_VULKAN_UTILITIES_H
