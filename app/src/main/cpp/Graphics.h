/*
 * Copyright (C) 2022 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <android/asset_manager.h>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>

#include "Utilities.h"

namespace vkt {
    std::vector<uint8_t> LoadBinaryFileToVector(const char *file_path,
                                                AAssetManager *assetManager) {
        std::vector<uint8_t> file_content;
        assert(assetManager);
        AAsset *file =
                AAssetManager_open(assetManager, file_path, AASSET_MODE_BUFFER);
        size_t file_length = AAsset_getLength(file);

        file_content.resize(file_length);

        AAsset_read(file, file_content.data(), file_length);
        AAsset_close(file);
        return file_content;
    }

    const char *toStringMessageSeverity(VkDebugUtilsMessageSeverityFlagBitsEXT s) {
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

    const char *toStringMessageType(VkDebugUtilsMessageTypeFlagsEXT s) {
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

    static VKAPI_ATTR VkBool32 VKAPI_CALL
    debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                  VkDebugUtilsMessageTypeFlagsEXT messageType,
                  const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                  void * /* pUserData */) {
        auto ms = toStringMessageSeverity(messageSeverity);
        auto mt = toStringMessageType(messageType);
        printf("[%s: %s]\n%s\n", ms, mt, pCallbackData->pMessage);

        return VK_FALSE;
    }

    static void populateDebugMessengerCreateInfo(
            VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }

    static VkResult CreateDebugUtilsMessengerEXT(
            VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
            const VkAllocationCallbacks *pAllocator,
            VkDebugUtilsMessengerEXT *pDebugMessenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
                instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    static void DestroyDebugUtilsMessengerEXT(
            VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
            const VkAllocationCallbacks *pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
                instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    class Graphics {
    public:

        PFN_vkCmdBeginRenderingKHR vkCmdBeginRenderingKHR{VK_NULL_HANDLE};
        PFN_vkCmdEndRenderingKHR vkCmdEndRenderingKHR{VK_NULL_HANDLE};

        VkPhysicalDeviceDynamicRenderingFeaturesKHR enabledDynamicRenderingFeaturesKHR{};

        void initVulkan();

        void render();

        void cleanup();

        void cleanupSwapChain();

        void reset(ANativeWindow *newWindow, AAssetManager *newManager);

        bool initialized = false;

    private:
        void createDevice();

        void createInstance();

        void createSurface();

        void setupDebugMessenger();

        void pickPhysicalDevice();

        void createLogicalDeviceAndQueue();

        void createSwapChain();

        void createImageViews();

        void createRenderPass();

        void createDescriptorSetLayout();

        void createGraphicsPipeline();

        void createFramebuffers();

        void createCommandPool();

        void createCommandBuffer();

        void createSyncObjects();

        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

        bool checkDeviceExtensionSupport(VkPhysicalDevice device);

        bool isDeviceSuitable(VkPhysicalDevice device);

        bool checkValidationLayerSupport();

        std::vector<const char *> getRequiredExtensions(bool enableValidation);

        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

        VkShaderModule createShaderModule(const std::vector<uint8_t> &code);

        void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

        void recreateSwapChain();

        void onOrientationChange();

        uint32_t findMemoryType(uint32_t typeFilter,
                                VkMemoryPropertyFlags properties);

        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                          VkMemoryPropertyFlags properties, VkBuffer &buffer,
                          VkDeviceMemory &bufferMemory);

        void createUniformBuffers();

        void updateUniformBuffer(uint32_t currentImage);

        void createDescriptorPool();

        void createDescriptorSets();

        void establishDisplaySizeIdentity();

        bool enableValidationLayers = false;

        const std::vector<const char *> validationLayers = {
                "VK_LAYER_KHRONOS_validation"};
        const std::vector<const char *> deviceExtensions = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
                VK_KHR_MAINTENANCE2_EXTENSION_NAME,
                VK_KHR_MULTIVIEW_EXTENSION_NAME,
                VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
                VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME
        };
        std::unique_ptr<ANativeWindow, ANativeWindowDeleter> window;
        AAssetManager *assetManager;

        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;

        VkSurfaceKHR surface;

        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device;

        VkSwapchainKHR swapChain;
        std::vector<VkImage> swapChainImages;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
        VkExtent2D displaySizeIdentity;
        std::vector<VkImageView> swapChainImageViews;
        std::vector<VkFramebuffer> swapChainFramebuffers;
        VkCommandPool commandPool;
        std::vector<VkCommandBuffer> commandBuffers;

        VkQueue graphicsQueue;
        VkQueue presentQueue;

        VkRenderPass renderPass;
        VkDescriptorSetLayout descriptorSetLayout;
        VkPipelineLayout pipelineLayout;
        VkPipeline graphicsPipeline;

        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        VkDescriptorPool descriptorPool;
        std::vector<VkDescriptorSet> descriptorSets;

        uint32_t currentFrame = 0;
        bool orientationChanged = false;
        VkSurfaceTransformFlagBitsKHR pretransformFlag;
    };
}