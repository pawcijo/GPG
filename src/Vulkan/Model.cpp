#include "Model.hpp"

#include <tiny_obj_loader.h>
#include <stb_image.h>

#include <vulkan/vulkan.h>
#include <Vulkan/VulkanUtis.hpp>

#include <filesystem>

namespace GPGVulkan
{

    uint32_t findMemoryType(VkPhysicalDevice aPhysicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(aPhysicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    VkImageView CreateImageView(VkDevice aDevice, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t aMipLevel)
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = aMipLevel;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VkImageView imageView;
        if (vkCreateImageView(aDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture image view!");
        }

        return imageView;
    }

    void CreateImage(VkDevice aDevice, VkPhysicalDevice aPhysicalDevice, uint32_t width, uint32_t height, uint32_t aMipLevels,
                     VkSampleCountFlagBits numSamples,
                     VkFormat format,
                     VkImageTiling tiling,
                     VkImageUsageFlags usage,
                     VkMemoryPropertyFlags properties,
                     VkImage &image,
                     VkDeviceMemory &imageMemory)
    {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = aMipLevels;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = numSamples;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(aDevice, &imageInfo, nullptr, &image) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(aDevice, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(aPhysicalDevice, memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(aDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(aDevice, image, imageMemory, 0);
    }

    void GenerateMipmaps(VkDevice aDevice,
                         VkPhysicalDevice aPhysicalDevice,
                         VkCommandPool aCommandPool,
                         VkQueue aGraphicsQueue,
                         VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t aMipLevels)
    {
        // Check if image format supports linear blitting
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(aPhysicalDevice, imageFormat, &formatProperties);

        if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
        {
            throw std::runtime_error("texture image format does not support linear blitting!");
        }

        VkCommandBuffer commandBuffer = BeginSingleTimeCommands(aCommandPool, aDevice);

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        int32_t mipWidth = texWidth;
        int32_t mipHeight = texHeight;

        for (uint32_t i = 1; i < aMipLevels; i++)
        {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                                 0, nullptr,
                                 0, nullptr,
                                 1, &barrier);

            VkImageBlit blit{};
            blit.srcOffsets[0] = {0, 0, 0};
            blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = {0, 0, 0};
            blit.dstOffsets[1] = {mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1};
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            vkCmdBlitImage(commandBuffer,
                           image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                           image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1, &blit,
                           VK_FILTER_LINEAR);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                                 0, nullptr,
                                 0, nullptr,
                                 1, &barrier);

            if (mipWidth > 1)
                mipWidth /= 2;
            if (mipHeight > 1)
                mipHeight /= 2;
        }

        barrier.subresourceRange.baseMipLevel = aMipLevels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                             0, nullptr,
                             0, nullptr,
                             1, &barrier);

        EndSingleTimeCommands(commandBuffer, aCommandPool, aDevice, aGraphicsQueue);
    }

    void CopyBufferToImage(VkDevice aDevice, VkCommandPool aCommandPool, VkQueue aGraphicsQueue, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
    {
        VkCommandBuffer commandBuffer = BeginSingleTimeCommands(aCommandPool, aDevice);

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {
            width,
            height,
            1};

        vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        EndSingleTimeCommands(commandBuffer, aCommandPool, aDevice, aGraphicsQueue);
    }

    void TransitionImageLayout(VkDevice aDevice, VkCommandPool aCommandPool, VkQueue aGraphicsQueue, uint32_t aMipLevels, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels)
    {

        VkCommandBuffer commandBuffer = BeginSingleTimeCommands(aCommandPool, aDevice);

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = aMipLevels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else
        {
            throw std::invalid_argument("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        EndSingleTimeCommands(commandBuffer, aCommandPool, aDevice, aGraphicsQueue);
    }

    void CreateBuffer(VkDevice aDevice,
                      VkPhysicalDevice aPhysicalDevice,
                      VkDeviceSize size,
                      VkBufferUsageFlags usage,
                      VkMemoryPropertyFlags properties,
                      VkBuffer &buffer,
                      VkDeviceMemory &bufferMemory)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(aDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(aDevice, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(aPhysicalDevice, memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(aDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        vkBindBufferMemory(aDevice, buffer, bufferMemory, 0);
    }
    void CopyBuffer(VkDevice aDevice,
                    VkBuffer srcBuffer,
                    VkBuffer dstBuffer,
                    VkDeviceSize size,
                    VkCommandPool aCommandPool,
                    VkQueue aGraphicsQueue)
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = aCommandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(aDevice, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(aGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(aGraphicsQueue);

        vkFreeCommandBuffers(aDevice, aCommandPool, 1, &commandBuffer);
    }

    void Model::createTextureImage(VkDevice aDevice, VkPhysicalDevice aPhysicalDevice, VkCommandPool aCommandPool, VkQueue aGraphicsQueue)
    {
        int texWidth, texHeight, texChannels;
        stbi_uc *pixels = stbi_load(mTexturePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;
        mMipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

        if (!pixels)
        {
            throw std::runtime_error("failed to load texture image!");
        }

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        CreateBuffer(aDevice, aPhysicalDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void *data;
        vkMapMemory(aDevice, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(aDevice, stagingBufferMemory);

        stbi_image_free(pixels);

        CreateImage(aDevice, aPhysicalDevice, texWidth, texHeight, mMipLevels,
                    VK_SAMPLE_COUNT_1_BIT,
                    VK_FORMAT_R8G8B8A8_SRGB,
                    VK_IMAGE_TILING_OPTIMAL,
                    VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mTextureImage, mTextureImageMemory);

        TransitionImageLayout(aDevice, aCommandPool, aGraphicsQueue, mMipLevels, mTextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mMipLevels);
        CopyBufferToImage(aDevice, aCommandPool, aGraphicsQueue, stagingBuffer, mTextureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
        // transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps

        vkDestroyBuffer(aDevice, stagingBuffer, nullptr);
        vkFreeMemory(aDevice, stagingBufferMemory, nullptr);

        GenerateMipmaps(aDevice, aPhysicalDevice, aCommandPool, aGraphicsQueue, mTextureImage, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, mMipLevels);
    }
    void Model::createTextureImageView(VkDevice aDevice)
    {
        mTextureImageView = CreateImageView(aDevice, mTextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mMipLevels);
    }

    Model::Model(std::filesystem::path aModelPath,
                 std::filesystem::path aTexturePath,
                 Transform aTransform,
                 VulkanContext &aContext) : mModelPath(aModelPath), mTexturePath(aTexturePath), mTransform(aTransform)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, aModelPath.c_str()))
        {
            throw std::runtime_error(warn + err);
        }

        std::unordered_map<Vertex, uint32_t> uniqueVertices{};

        for (const auto &shape : shapes)
        {
            for (const auto &index : shape.mesh.indices)
            {
                Vertex vertex{};

                vertex.pos = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]};

                vertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]};

                vertex.color = {1.0f, 1.0f, 1.0f};

                if (uniqueVertices.count(vertex) == 0)
                {
                    uniqueVertices[vertex] = static_cast<uint32_t>(mVertices.size());
                    mVertices.push_back(vertex);
                }

                mIndices.push_back(uniqueVertices[vertex]);
            }
        }

        createVertexBuffer(aContext.mDevice, aContext.mPhysicalDevice, aContext.mCommandPool, aContext.mGraphicsQueue);
        createIndexBuffer(aContext.mDevice, aContext.mPhysicalDevice, aContext.mCommandPool, aContext.mGraphicsQueue);

        createTextureImage(aContext.mDevice, aContext.mPhysicalDevice, aContext.mCommandPool, aContext.mGraphicsQueue);
        createTextureImageView(aContext.mDevice);

        createDescriptorSets(aContext.mDevice, aContext.mTextureSampler, aContext.mDescriptorPool, aContext.mDescriptorSetLayout, aContext.mUniformBuffers);
    }

    Model::Model(std::filesystem::path modelPath,
                 std::filesystem::path aTexturePath,
                 VulkanContext &aContext) : mModelPath(modelPath), mTexturePath(aTexturePath), mTransform(Transform::origin())
    {

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath.c_str()))
        {
            throw std::runtime_error(warn + err);
        }

        std::unordered_map<Vertex, uint32_t> uniqueVertices{};

        std::vector<Vertex> mVertices;
        std::vector<uint32_t> mIndices;

        for (const auto &shape : shapes)
        {
            for (const auto &index : shape.mesh.indices)
            {
                Vertex vertex{};

                vertex.pos = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]};

                vertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]};

                vertex.color = {1.0f, 1.0f, 1.0f};

                if (uniqueVertices.count(vertex) == 0)
                {
                    uniqueVertices[vertex] = static_cast<uint32_t>(mVertices.size());
                    mVertices.push_back(vertex);
                }

                mIndices.push_back(uniqueVertices[vertex]);
            }
        }

        createVertexBuffer(aContext.mDevice, aContext.mPhysicalDevice, aContext.mCommandPool, aContext.mGraphicsQueue);
        createIndexBuffer(aContext.mDevice, aContext.mPhysicalDevice, aContext.mCommandPool, aContext.mGraphicsQueue);

        createTextureImage(aContext.mDevice, aContext.mPhysicalDevice, aContext.mCommandPool, aContext.mGraphicsQueue);
        createTextureImageView(aContext.mDevice);

        createDescriptorSets(aContext.mDevice, aContext.mTextureSampler, aContext.mDescriptorPool, aContext.mDescriptorSetLayout, aContext.mUniformBuffers);
    }

    std::vector<Vertex> &Model::Vertices()
    {
        return mVertices;
    }
    std::vector<uint32_t> &Model::Indices()
    {
        return mIndices;
    }

    void Model::createVertexBuffer(VkDevice aDevice, VkPhysicalDevice aPhysicalDevice, VkCommandPool aCommandPool,
                                   VkQueue aGraphicsQueue)
    {

        VkDeviceSize bufferSize = sizeof(mVertices[0]) * mVertices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        CreateBuffer(aDevice, aPhysicalDevice, bufferSize,
                     VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     stagingBuffer, stagingBufferMemory);

        void *data;
        vkMapMemory(aDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, mVertices.data(), (size_t)bufferSize);
        vkUnmapMemory(aDevice, stagingBufferMemory);

        CreateBuffer(aDevice, aPhysicalDevice, bufferSize,
                     VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mVertexBuffer, mVertexBufferMemory);

        CopyBuffer(aDevice, stagingBuffer, mVertexBuffer, bufferSize, aCommandPool, aGraphicsQueue);

        vkDestroyBuffer(aDevice, stagingBuffer, nullptr);
        vkFreeMemory(aDevice, stagingBufferMemory, nullptr);
    }

    void Model::createIndexBuffer(VkDevice aDevice, VkPhysicalDevice aPhysicalDevice, VkCommandPool aCommandPool,
                                  VkQueue aGraphicsQueue)
    {
        VkDeviceSize bufferSize = sizeof(mIndices[0]) * mIndices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        CreateBuffer(aDevice, aPhysicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void *data;
        vkMapMemory(aDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, mIndices.data(), (size_t)bufferSize);
        vkUnmapMemory(aDevice, stagingBufferMemory);

        CreateBuffer(aDevice, aPhysicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mIndexBuffer, mIndexBufferMemory);

        CopyBuffer(aDevice, stagingBuffer, mIndexBuffer, bufferSize, aCommandPool, aGraphicsQueue);

        vkDestroyBuffer(aDevice, stagingBuffer, nullptr);
        vkFreeMemory(aDevice, stagingBufferMemory, nullptr);
    }

    void Model::createDescriptorSets(VkDevice aDevice,
                                     VkSampler aSampler,
                                     VkDescriptorPool aDescriptorPool,
                                     VkDescriptorSetLayout aDescriptorSetLayout,
                                     std::vector<VkBuffer> aUniformBuffers)
    {
        int MAX_FRAMES_IN_FLIGHT = 2;
        std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, aDescriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = aDescriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        allocInfo.pSetLayouts = layouts.data();

        mDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
        VkResult result = vkAllocateDescriptorSets(aDevice, &allocInfo, mDescriptorSets.data());
        if (result != VK_SUCCESS)
        {

            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = aUniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            imageInfo.imageView = mTextureImageView;
            imageInfo.sampler = aSampler;

            std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = mDescriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = mDescriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo = &imageInfo;

            vkUpdateDescriptorSets(aDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }

    std::vector<VkDescriptorSet> Model::DescriptorSets()
    {
        return mDescriptorSets;
    }

    VkBuffer Model::VertexBuffer()
    {
        return mVertexBuffer;
    };
    VkBuffer Model::IndexBuffer()
    {
        return mIndexBuffer;
    }

    VkImageView Model::TextureImageView()
    {
        return mTextureImageView;
    }

    std::filesystem::path Model::ModelPath()
    {
        return mModelPath;
    }
    std::filesystem::path Model::TexturePath()
    {
        return mTexturePath;
    }

    long long Model::GetModelSizeInBytes()
    {
        long long modelSizeInBytes = 0;

        modelSizeInBytes += sizeof(mModelPath);
        modelSizeInBytes += sizeof(mTexturePath);

        modelSizeInBytes += sizeof(mVertices[0]) * mVertices.size();
        modelSizeInBytes += sizeof(mIndices[0]) * mIndices.size();

        // TODO add missing texture stuff

        return modelSizeInBytes;
    }

    void Model::CleanUpTextures(VkDevice aDevice)
    {

        vkDestroyImageView(aDevice, mTextureImageView, nullptr);

        vkDestroyImage(aDevice, mTextureImage, nullptr);
        vkFreeMemory(aDevice, mTextureImageMemory, nullptr);
    }

    void Model::CleanUp(VkDevice aDevice)
    {
        vkDestroyBuffer(aDevice, mIndexBuffer, nullptr);
        vkFreeMemory(aDevice, mIndexBufferMemory, nullptr);

        vkDestroyBuffer(aDevice, mVertexBuffer, nullptr);
        vkFreeMemory(aDevice, mVertexBufferMemory, nullptr);
    }

    void Model::RecordModelDraw(VkCommandBuffer aCommandBuffer,
                                uint32_t aCurrentFrame,
                                VkPipelineLayout aPipelineLayout)
    {

        VkBuffer vertexBuffers[] = {mVertexBuffer};

        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(aCommandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(aCommandBuffer, mIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdBindDescriptorSets(aCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, aPipelineLayout, 0, 1,
                                &mDescriptorSets[aCurrentFrame], 0, nullptr);

        MeshPushConstants constants;
        constants.render_matrix = mTransform.TransformMat4();

        vkCmdPushConstants(aCommandBuffer, aPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstants), &constants);
        vkCmdDrawIndexed(aCommandBuffer, static_cast<uint32_t>(mIndices.size()), 1, 0, 0, 0);
    }

}