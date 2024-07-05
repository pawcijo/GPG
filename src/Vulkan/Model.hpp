#pragma once

#include "Vulkan/VulkanUtis.hpp"
#include "Common/Transform.h"

#include <filesystem>

class Model
{
    std::vector<Vertex> mVertices;
    std::vector<uint32_t> mIndices;

    VkBuffer mVertexBuffer;
    VkDeviceMemory mVertexBufferMemory;

    VkBuffer mIndexBuffer;
    VkDeviceMemory mIndexBufferMemory;

    Transform mTransform;

    void createVertexBuffer(VkDevice aDevice,
                            VkPhysicalDevice aPhysicalDevice,
                            VkCommandPool aCommandPool,
                            VkQueue aGraphicsQueue);
    void createIndexBuffer(VkDevice aDevice,
                           VkPhysicalDevice aPhysicalDevice,
                           VkCommandPool aCommandPool,
                           VkQueue aGraphicsQueue);

public:
    Model(std::filesystem::path modelPath, VkDevice aDevice, VkPhysicalDevice aPhysicalDevice, VkCommandPool aCommandPool,
          VkQueue aGraphicsQueue);

    void CleanUp(VkDevice aDevice);

    std::vector<Vertex> &Vertices();
    std::vector<uint32_t> &Indices();

    Transform &GetTransform() { return mTransform; }

    VkBuffer VertexBuffer();
    VkBuffer IndexBuffer();
};