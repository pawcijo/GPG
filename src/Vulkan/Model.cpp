#include "Model.hpp"

#include <tiny_obj_loader.h>

#include <Vulkan/VulkanUtis.hpp>

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

Model::Model(std::filesystem::path modelPath,
             VkDevice aDevice,
             VkPhysicalDevice aPhysicalDevice,
             VkCommandPool aCommandPool,
             VkQueue aGraphicsQueue)
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

    createVertexBuffer(aDevice, aPhysicalDevice, aCommandPool, aGraphicsQueue);
    createIndexBuffer(aDevice, aPhysicalDevice, aCommandPool, aGraphicsQueue);
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

VkBuffer Model::VertexBuffer()
{
    return mVertexBuffer;
};
VkBuffer Model::IndexBuffer()
{
    return mIndexBuffer;
}

void Model::CleanUp(VkDevice aDevice)
{
    vkDestroyBuffer(aDevice, mIndexBuffer, nullptr);
    vkFreeMemory(aDevice, mIndexBufferMemory, nullptr);

    vkDestroyBuffer(aDevice, mVertexBuffer, nullptr);
    vkFreeMemory(aDevice, mVertexBufferMemory, nullptr);
}