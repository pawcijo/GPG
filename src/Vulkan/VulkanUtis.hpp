#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <array>
#include <string>
#include <vector>
#include <fstream>

#include <unordered_map>

namespace GPGVulkan
{
    struct VulkanContext
    {

        VkInstance mInstance;
        VkAllocationCallbacks *g_Allocator = nullptr;
        VkDebugUtilsMessengerEXT mDebugMessenger;

        VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
        VkDevice mDevice;

        VkQueue mGraphicsQueue;
        VkQueue mPresentQueue;

        VkSurfaceKHR mSurface;

        VkSwapchainKHR mSwapChain;
        std::vector<VkImage> mSwapChainImages;
        VkFormat mSwapChainImageFormat;
        VkExtent2D mSwapChainExtent;
        std::vector<VkImageView> mSwapChainImageViews;
        std::vector<VkFramebuffer> mSwapChainFramebuffers;

        VkRenderPass mRenderPass;
        VkDescriptorSetLayout mDescriptorSetLayout;
        VkPipelineLayout mPipelineLayout;
        VkPipeline mGraphicsPipeline;

        VkCommandPool mCommandPool;
        std::vector<VkCommandBuffer> mCommandBuffers;

        std::vector<VkSemaphore> mImageAvailableSemaphores;
        std::vector<VkSemaphore> mRenderFinishedSemaphores;
        std::vector<VkFence> mInFlightFences;

        std::vector<VkBuffer> mUniformBuffers;
        std::vector<VkDeviceMemory> mUniformBuffersMemory;
        std::vector<void *> mUniformBuffersMapped;

        VkDescriptorPool mImguiDescriptorPool;
        VkDescriptorPool mDescriptorPool;
        std::vector<VkDescriptorSet> mDescriptorSets;

        uint32_t mMipLevels;
        VkSampler mTextureSampler;

        VkSampleCountFlagBits mMsaaSamples = VK_SAMPLE_COUNT_1_BIT;

        VkImage mDepthImage;
        VkDeviceMemory mDepthImageMemory;
        VkImageView mDepthImageView;

        VkImage mColorImage;
        VkDeviceMemory mColorImageMemory;
        VkImageView mColorImageView;
    };

    struct UniformBufferObject
    {
        glm::mat4 view;
        glm::mat4 proj;
    };

    struct MeshPushConstants
    {
        glm::mat4 render_matrix;
    };

    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 texCoord;

        static VkVertexInputBindingDescription getBindingDescription()
        {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
        {
            std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, pos);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, color);

            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

            return attributeDescriptions;
        }

        bool operator==(const Vertex &other) const
        {
            return pos == other.pos && color == other.color && texCoord == other.texCoord;
        }
    };

    static std::vector<char> readFile(const std::string &filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open())
        {
            std::string error = "failed to open file : " + filename;
            throw std::runtime_error(error);
        }

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }
    VkCommandBuffer BeginSingleTimeCommands(VkCommandPool aCommandPool, VkDevice aDevice);
    void EndSingleTimeCommands(VkCommandBuffer commandBuffer, VkCommandPool aCommandPool, VkDevice aDevice, VkQueue aGraphicQueue);

}

namespace std
{
    template <>
    struct hash<GPGVulkan::Vertex>
    {
        size_t operator()(GPGVulkan::Vertex const &vertex) const
        {
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}
