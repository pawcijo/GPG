#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <vector>
#include <optional>

namespace AppWindowVulkan
{

    const int MAX_FRAMES_IN_FLIGHT = 2;

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete()
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    class AppWindowVulkan
    {
        GLFWwindow *mWindow;
        VkInstance mInstance;
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

        VkBuffer mVertexBuffer;
        VkDeviceMemory mVertexBufferMemory;

        VkBuffer mIndexBuffer;
        VkDeviceMemory mIndexBufferMemory;

        std::vector<VkBuffer> mUniformBuffers;
        std::vector<VkDeviceMemory> mUniformBuffersMemory;
        std::vector<void *> mUniformBuffersMapped;

        VkDescriptorPool mDescriptorPool;
        std::vector<VkDescriptorSet> mDescriptorSets;

        VkImage mTextureImage;
        VkDeviceMemory mTextureImageMemory;
        VkImageView mTextureImageView;
        VkSampler mTextureSampler;

        VkImage mDepthImage;
        VkDeviceMemory mDepthImageMemory;
        VkImageView mDepthImageView;

        unsigned int mWidth;
        unsigned int mHeight;
        void initWindow(int width, int height);
        void initVulkan();

        uint32_t mCurrentFrame = 0;

        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice aDevice);
        bool isDeviceSuitable(VkPhysicalDevice aDevice);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice aPhysicalDevice);
        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        void updateUniformBuffer(uint32_t currentImage);

        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
        void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory);
        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

        VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
        VkFormat findDepthFormat();

        void createInstance();
        void setupDebugMessenger();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createSwapChain();
        void createImageViews();
        void createRenderPass();
        void createDescriptorSetLayout();
        void createGraphicsPipeline();
        void createFramebuffers();
        void createCommandPool();
        void createDepthResources();
        void createTextureImage();
        void createTextureImageView();
        void createTextureSampler();
        void createVertexBuffer();
        void createIndexBuffer();
        void createUniformBuffers();
        void createDescriptorPool();
        void createDescriptorSets();
        void createCommandBuffers();
        void createSyncObjects();

        void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
        void cleanupSwapChain();

        void recreateSwapChain();

    public:
        AppWindowVulkan(unsigned int width, unsigned int height);
        GLFWwindow *GetWindow();
        VkInstance GetInstance();
        unsigned int GetWidth() { return mWidth; }
        unsigned int Getheight() { return mHeight; }

        VkDevice GetDevice() { return mDevice; }
        bool mFramebufferResized = false;

        void DrawFrame();

        void CleanUp();
    };

}