#include "VulkanUtis.hpp"

namespace GPGVulkan
{
  
VkCommandBuffer BeginSingleTimeCommands(VkCommandPool aCommandPool, VkDevice aDevice)
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

    return commandBuffer;
}

void EndSingleTimeCommands(VkCommandBuffer commandBuffer, VkCommandPool aCommandPool, VkDevice aDevice, VkQueue aGraphicQueue)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(aGraphicQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(aGraphicQueue);

    vkFreeCommandBuffers(aDevice, aCommandPool, 1, &commandBuffer);
}


}