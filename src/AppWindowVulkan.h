#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <vector>
#include <optional>

namespace AppWindowVulkan
{

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
        VkPipelineLayout mPipelineLayout;
        VkPipeline mGraphicsPipeline;

        VkCommandPool mCommandPool;
        VkCommandBuffer mCommandBuffer;

        VkSemaphore mImageAvailableSemaphore;
        VkSemaphore mRenderFinishedSemaphore;
        VkFence mInFlightFence;

        unsigned int mWidth;
        unsigned int mHeight;
        void InitWindow(int width, int height);
        void InitVulkan();

        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice aDevice);
        bool isDeviceSuitable(VkPhysicalDevice aDevice);

        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

        SwapChainSupportDetails querySwapChainSupport();

        void createInstance();
        void setupDebugMessenger();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createSwapChain();
        void createImageViews();
        void createRenderPass();
        void createGraphicsPipeline();
        void createFramebuffers();
        void createCommandPool();
        void createCommandBuffer();
        void createSyncObjects();

        void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    public:
        AppWindowVulkan(unsigned int width, unsigned int height);
        GLFWwindow *GetWindow();
        VkInstance GetInstance();
        unsigned int GetWidth() { return mWidth; }
        unsigned int Getheight() { return mHeight; }

        VkDevice GetDevice(){return mDevice;}

        void DrawFrame();

        void CleanUp();
    };

}