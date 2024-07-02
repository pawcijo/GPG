#pragma once

#include <Vulkan/VulkanUtis.hpp>
#include <filesystem>

class Model
{
    std::vector<Vertex> mVertices;
    std::vector<uint32_t> mIndices;

    VkBuffer mVertexBuffer;
    VkDeviceMemory mVertexBufferMemory;

    VkBuffer mIndexBuffer;
    VkDeviceMemory mIndexBufferMemory;

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

    VkBuffer VertexBuffer();

    VkBuffer IndexBuffer();
};