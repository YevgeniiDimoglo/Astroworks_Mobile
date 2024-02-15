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


const int MAX_FRAMES_IN_FLIGHT = 2;

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

struct ANativeWindowDeleter {
    void operator()(ANativeWindow *window) { ANativeWindow_release(window); }
};

#endif //HELLO_VULKAN_UTILITIES_H
