#pragma once

#include "Vulkan/VulkanUtis.hpp"
#include "Common/Transform.h"

#include <filesystem>

namespace GPGVulkan
{

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
        VkSampler mTextureSampler;

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
        void createTextureSampler(VkDevice aDevice, VkPhysicalDevice aPhysicalDevice);

    public:
        Model(std::filesystem::path aModelPath,
              std::filesystem::path aTexturePath,
              VkDevice aDevice,
              VkPhysicalDevice aPhysicalDevice,
              VkCommandPool aCommandPool,
              VkQueue aGraphicsQueue);

        void CleanUp(VkDevice aDevice);
        void CleanUpTextures(VkDevice aDevice);

        void serialize(std::ofstream &outFile) const;

        std::vector<Vertex> &Vertices();
        std::vector<uint32_t> &Indices();

        Transform &GetTransform() { return mTransform; }

        VkBuffer VertexBuffer();
        VkBuffer IndexBuffer();

        VkImageView TextureImageView();
        VkSampler TextureSampler();
    };

}