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

        VkSwapchainKHR swapChain;
        std::vector<VkImage> swapChainImages;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
        std::vector<VkImageView> swapChainImageViews;

        unsigned int mWidth;
        unsigned int mHeight;
        void InitWindow(int width, int height);
        void InitVulkan();

        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice aDevice);
        bool isDeviceSuitable(VkPhysicalDevice aDevice);

        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

        SwapChainSupportDetails querySwapChainSupport();

        void createInstance();

        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createSwapChain();
        void createImageViews();
        void createGraphicsPipeline();

    public:
        AppWindowVulkan(unsigned int width, unsigned int height);
        GLFWwindow *GetWindow();
        VkInstance GetInstance();
        unsigned int GetWidth() { return mWidth; }
        unsigned int Getheight() { return mHeight; }

        void CleanUp();
    };

}