#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>
#include "Vulkan/VulkanUtis.hpp"

#include "Common/Camera.h"

#include <vector>
#include <optional>
#include <string>

#include "Vulkan/Model.hpp"

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

class VulkanPipeLine
{
    GLFWwindow *mWindow;
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
    VkImage mTextureImage;
    VkDeviceMemory mTextureImageMemory;
    VkImageView mTextureImageView;
    VkSampler mTextureSampler;

    VkSampleCountFlagBits mMsaaSamples = VK_SAMPLE_COUNT_1_BIT;

    VkImage mDepthImage;
    VkDeviceMemory mDepthImageMemory;
    VkImageView mDepthImageView;

    VkImage mColorImage;
    VkDeviceMemory mColorImageMemory;
    VkImageView mColorImageView;

    unsigned int mWidth;
    unsigned int mHeight;
    void initWindow(int width, int height);
    void initVulkan();

    // Imgui
    //  Setup Dear ImGui context

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    uint32_t mCurrentFrame = 0;

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice aDevice);
    bool isDeviceSuitable(VkPhysicalDevice aDevice);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice aPhysicalDevice);
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    void updateUniformBuffer(uint32_t currentImage, Camera &camera);

    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
    void createImage(uint32_t width, uint32_t height, uint32_t aMipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory);
    void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlagsm, uint32_t mipLevels);

    VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    VkFormat findDepthFormat();

    VkSampleCountFlagBits getMaxUsableSampleCount();

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
    void createColorResources();
    void createDepthResources();
    void createTextureImage();
    void createTextureImageView();
    void createTextureSampler();

    void setupImgui();

    void loadModel();

    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets();
    void createCommandBuffers();
    void createSyncObjects();

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void cleanupSwapChain();

    void recreateSwapChain();

    void drawImgui(VkCommandBuffer commandBuffer);

    // move to App or even better to config/project save file.
    const std::string MODEL_PATH = "resources/models/viking_room.obj";
    const std::string TEXTURE_PATH = "resources/textures/viking_room.png";

    // move to app class
    std::unique_ptr<Model> model;

public:
    VulkanPipeLine(unsigned int width, unsigned int height);
    GLFWwindow *GetWindow();
    VkInstance GetInstance();
    unsigned int GetWidth() { return mWidth; }
    unsigned int Getheight() { return mHeight; }

    VkDevice GetDevice() { return mDevice; }
    bool mFramebufferResized = false;

    void DrawFrame(Camera &aCamera);

    void CleanUp();
};
