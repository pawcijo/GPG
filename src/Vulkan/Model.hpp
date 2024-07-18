#pragma once

#include "Vulkan/VulkanUtis.hpp"
#include "Vulkan/VulkanPipeLine.hpp"
#include "Common/Transform.h"

#include <filesystem>

namespace GPGVulkan
{

    class VulkanApp;
    class VulkanPipeline;
    class Model
    {
        std::vector<Vertex> mVertices;
        std::vector<uint32_t> mIndices;

        VkBuffer mVertexBuffer;
        VkDeviceMemory mVertexBufferMemory;

        VkBuffer mIndexBuffer;
        VkDeviceMemory mIndexBufferMemory;

        std::filesystem::path mModelPath;
        std::filesystem::path mTexturePath;
        Transform mTransform;

        uint32_t mMipLevels;
        VkImage mTextureImage;
        VkDeviceMemory mTextureImageMemory;
        VkImageView mTextureImageView;

        std::vector<VkDescriptorSet> mDescriptorSets; // texture defaulted to null

        void createVertexBuffer(VkDevice aDevice,
                                VkPhysicalDevice aPhysicalDevice,
                                VkCommandPool aCommandPool,
                                VkQueue aGraphicsQueue);
        void createIndexBuffer(VkDevice aDevice,
                               VkPhysicalDevice aPhysicalDevice,
                               VkCommandPool aCommandPool,
                               VkQueue aGraphicsQueue);

        void createTextureImage(VkDevice aDevice,
                                VkPhysicalDevice aPhysicalDevice,
                                VkCommandPool aCommandPool,
                                VkQueue aGraphicsQueue);
        void createTextureImageView(VkDevice aDevice);

        void createDescriptorSets(VkDevice aDevice,
                                  VkSampler aSampler,
                                  VkDescriptorPool aDescriptorPool,
                                  VkDescriptorSetLayout aDescriptorSetLayout,
                                  std::vector<VkBuffer> mUniformBuffers);

    public:
        Model(std::filesystem::path aModelPath,
              std::filesystem::path aTexturePath,
              VulkanContext &aContext);

        Model(std::filesystem::path aModelPath,
              std::filesystem::path aTexturePath,
              Transform aTransform,
              VulkanContext &aContext);

        void CleanUp(VkDevice aDevice);
        void CleanUpTextures(VkDevice aDevice);

        std::filesystem::path ModelPath();
        std::filesystem::path TexturePath();

        std::vector<Vertex> &Vertices();
        std::vector<uint32_t> &Indices();

        Transform &GetTransform() { return mTransform; }

        VkBuffer VertexBuffer();
        VkBuffer IndexBuffer();

        VkImageView TextureImageView();
        std::vector<VkDescriptorSet> DescriptorSets();

        void RecordModelDraw(VkCommandBuffer aCommandBuffer,
                             uint32_t aCurrentFrame, VkPipelineLayout aPipelineLayout);

        long long GetModelSizeInBytes();

        friend class VulkanApp;
        friend class VulkanPipeline;
    };

}