//
// Created by Yujin on 2024/02/21.
//

#include "Sprite.h"

#include "external/stb_image.h"

Sprite::Sprite(std::string filePath)
{
    std::size_t found = filePath.find_last_of("/\\");
    std::string filePathTemp = filePath.substr(0, found);
    std::string fileNameType = filePath.substr(found + 1);

    found = fileNameType.find_last_of(".");
    std::string fileName = fileNameType.substr(0, found);
    std::string fileType = fileNameType.substr(found + 1);

    this->filePath = filePath;
    this->fileName = fileName;
    this->fileType = fileType;
}

void Sprite::loadFile(VkPhysicalDevice newPhysicalDevice, VkDevice newLogicalDevice, VkQueue transferQueue, VkCommandPool transferCommandPool, VkDescriptorPool samplerDescriptorPool, VkDescriptorSetLayout samplerSetLayout)
{
    this->newPhysicalDevice = newPhysicalDevice;
    this->newLogicalDevice = newLogicalDevice;
    this->transferQueue = transferQueue;
    this->commandPool = transferCommandPool;
    this->samplerDescriptorPool = samplerDescriptorPool;
    this->samplerSetLayout = samplerSetLayout;

    std::vector<Sprite::Vertex>				vertexBuffer;
    std::vector<uint32_t>					indexBuffer;

    vertexBuffer =
            {
                    {{-1.f,  -1.0f, 0.f}, {1.0f, 1.0f, 1.0f}, {0.f, 0.f}},
                    {{ 1.0f, -1.0f, 0.f}, {1.0f, 1.0f, 1.0f}, {1.f, 0.f}},
                    {{ 1.0f,  1.0f, 0.f}, {1.0f, 1.0f, 1.0f}, {1.f, 1.f}},
                    {{-1.0f,  1.0f, 0.f}, {1.0f, 1.0f, 1.0f}, {0.f, 1.f}}
            };

    indexBuffer =
            {
                    0, 1, 2, 2, 3, 0
            };

    int width, height;
    int channels;
    VkDeviceSize imageSize;

    // Load pixel data for image
    std::string fileLoc = filePath;
    stbi_uc* image = stbi_load(fileLoc.c_str(), &width, &height, &channels, STBI_rgb_alpha);

    LOG("Loading sprite: " + filePath + "\n");

    imageSize = width * height * 4;

    createTextureFromBuffer(image, imageSize, VK_FORMAT_R8G8B8A8_UNORM, width, height, newPhysicalDevice, newLogicalDevice, transferCommandPool, transferQueue);

    indices.count = static_cast<uint32_t>(indexBuffer.size());

    createVertexBuffer(newPhysicalDevice, newLogicalDevice, transferQueue, transferCommandPool, &vertexBuffer);
    createIndexBuffer(newPhysicalDevice, newLogicalDevice, transferQueue, transferCommandPool, &indexBuffer);
}

void Sprite::draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout)
{
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertices.buffer, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indices.buffer, 0, VK_INDEX_TYPE_UINT32);

    PushConstants pushConstants;
    pushConstants.baseColor = glm::vec4(r, g, b, a);
    pushConstants.model = spriteValues;
    vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstants), &pushConstants);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 1, 1, &image.descriptorSet, 0, nullptr);

    vkCmdDrawIndexed(commandBuffer, indices.count, 1, 0, 0, 0);
}

void Sprite::updateTransform()
{
    // Transform matrix
    glm::mat4x4 translate = glm::translate(glm::mat4x4(1.0f), glm::vec3(positionX, positionY, positionZ));

    glm::mat4x4 rotate = glm::rotate(glm::mat4x4(1.0), glm::radians(angle), glm::vec3(0.0f, 0.f, 1.f));

    glm::mat4x4 scale = glm::scale(glm::mat4x4(1.0f), glm::vec3(width, height, 0.f));

    glm::mat4x4 transform = translate * rotate * scale;
    spriteValues = transform;
}

void Sprite::cleanupResourses(VkDevice newLogicalDevice)
{
    vkDestroyImageView(newLogicalDevice, image.view, nullptr);
    vkDestroyImage(newLogicalDevice, image.image, nullptr);
    vkFreeMemory(newLogicalDevice, image.deviceMemory, nullptr);
    vkDestroySampler(newLogicalDevice, image.sampler, nullptr);

    vkDestroyBuffer(newLogicalDevice, vertices.buffer, nullptr);
    vkFreeMemory(newLogicalDevice, vertices.memory, nullptr);

    vkDestroyBuffer(newLogicalDevice, indices.buffer, nullptr);
    vkFreeMemory(newLogicalDevice, indices.memory, nullptr);
}

void Sprite::createVertexBuffer(VkPhysicalDevice newPhysicalDevice, VkDevice newLogicalDevice, VkQueue transferQueue, VkCommandPool transferCommandPool, std::vector<Vertex>* vertices)
{
    VkDeviceSize bufferSize = sizeof(Sprite::Vertex) * vertices->size();

    // Temporary buffer to "stage" vertex data before transfering to GPU
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    // Create Staging buffer and allocate memory to it
    createBuffer(newPhysicalDevice, newLogicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory);

    // Map memory to vertex buffer
    void* data;																					// 1. Create pointer to a point in normal memory
    vkMapMemory(newLogicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);							// 2, "Map" the vertex buffer memory to that point
    memcpy(data, vertices->data(), bufferSize);													// 3. Copy memory from vertices vector to the point
    vkUnmapMemory(newLogicalDevice, stagingBufferMemory);													// 4. Unmap the vertex buffer memory

    // Create buffer with TRANSFER_DST_BIT to mark as recipient of transfer data ( also VERTEX buffer)
    // Buffer memory is to be DEVICE_LOCAL_BIT meaning memory is on the GPU and only accessible by it and not CPU (host)
    createBuffer(newPhysicalDevice, newLogicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, this->vertices.buffer, this->vertices.memory);

    // Copy staging	buffer to vertex buffer on GPU
    copyBuffer(newLogicalDevice, transferCommandPool, transferQueue, stagingBuffer, this->vertices.buffer, bufferSize);

    vkDestroyBuffer(newLogicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(newLogicalDevice, stagingBufferMemory, nullptr);
}

void Sprite::createIndexBuffer(VkPhysicalDevice newPhysicalDevice, VkDevice newLogicalDevice, VkQueue transferQueue, VkCommandPool transferCommandPool, std::vector<uint32_t>* indices)
{
    // Get size of buffer needed for index
    VkDeviceSize bufferSize = sizeof(uint32_t) * indices->size();

    // Temporary buffer to "stage" index data before transfering to GPU
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    // Create Staging buffer and allocate memory to it
    createBuffer(newPhysicalDevice, newLogicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory);

    // Map memory to index  buffer
    void* data;
    vkMapMemory(newLogicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices->data(), (size_t)bufferSize);
    vkUnmapMemory(newLogicalDevice, stagingBufferMemory);

    // Create buffer for Index data on GPU access only area
    createBuffer(newPhysicalDevice, newLogicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, this->indices.buffer, this->indices.memory);

    // Copy staging	buffer to vertex buffer on GPU
    copyBuffer(newLogicalDevice, transferCommandPool, transferQueue, stagingBuffer, this->indices.buffer, bufferSize);

    // Destoy + release Staging buffer resources
    vkDestroyBuffer(newLogicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(newLogicalDevice, stagingBufferMemory, nullptr);
}

Sprite::Image Sprite::createTextureFromBuffer(void* buffer, VkDeviceSize bufferSize, VkFormat format, uint32_t texWidth, uint32_t texHeight, VkPhysicalDevice newPhysicalDevice, VkDevice newLogicalDevice, VkCommandPool transferCommandPool, VkQueue transferQueue)
{
    assert(buffer);

    image.width = texWidth;
    image.height = texHeight;

    VkBuffer imageStagingBuffer;
    VkDeviceMemory imageStagingBufferMemory;
    createBuffer(newPhysicalDevice, newLogicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 imageStagingBuffer, imageStagingBufferMemory);

    // Copy image data to staging buffer
    void* data;
    vkMapMemory(newLogicalDevice, imageStagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, buffer, static_cast<size_t>(bufferSize));
    vkUnmapMemory(newLogicalDevice, imageStagingBufferMemory);

    // Create image to hold final texture
    VkImage texImage;
    VkDeviceMemory texImageMemory;
    texImage = createImage(newPhysicalDevice, newLogicalDevice,
                           texWidth, texHeight, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
                           VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, texImageMemory);

    // Copy data to image
    // Transition image to be DST for copy operation
    transitionImageLayout(newLogicalDevice, transferCommandPool, transferQueue,
                          texImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1);

    // Copy image data
    copyBufferToImage(newLogicalDevice, transferCommandPool, transferQueue, imageStagingBuffer, texImage, texWidth, texHeight, 1);

    // Transition image to be shader readable for shader usage
    transitionImageLayout(newLogicalDevice, transferCommandPool, transferQueue,
                          texImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1);

    // Add texture data to vector for reference
    image.image = texImage;
    image.deviceMemory = texImageMemory;

    // Destroy staging buffers
    vkDestroyBuffer(newLogicalDevice, imageStagingBuffer, nullptr);
    vkFreeMemory(newLogicalDevice, imageStagingBufferMemory, nullptr);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;

    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = 1.0f;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    VkSampler imageSampler;
    vkCreateSampler(newLogicalDevice, &samplerInfo, nullptr, &imageSampler);

    image.sampler = imageSampler;

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = texImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    vkCreateImageView(newLogicalDevice, &viewInfo, nullptr, &imageView);

    image.view = imageView;

    VkDescriptorSet descriptorSet;

    // Descriptor set allocation info
    VkDescriptorSetAllocateInfo setAllocInfo = {};
    setAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    setAllocInfo.descriptorPool = samplerDescriptorPool;
    setAllocInfo.descriptorSetCount = 1;
    setAllocInfo.pSetLayouts = &samplerSetLayout;

    // Allocate descriptor sets
    VkResult result = vkAllocateDescriptorSets(newLogicalDevice, &setAllocInfo, &descriptorSet);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate texture descriptor sets");
    }

    // Texture image info
    VkDescriptorImageInfo imageInfo = {};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;							// Image layout when image in use
    imageInfo.imageView = imageView;															// Image to bind to set
    imageInfo.sampler = imageSampler;															// Sampler to use for set

    image.descriptor = imageInfo;

    // Descriptor write info
    VkWriteDescriptorSet descriptorWrite = {};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSet;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pImageInfo = &imageInfo;

    // Update new descriptor set
    vkUpdateDescriptorSets(newLogicalDevice, 1, &descriptorWrite, 0, nullptr);

    image.descriptorSet = descriptorSet;

    return image;
}