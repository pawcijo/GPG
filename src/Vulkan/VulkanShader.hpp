#pragma once
#include <vulkan/vulkan_core.h>
#include <string>

class VulkanShader
{
    std::string mName;
    VkStructureType mShaderType;
    VkShaderStageFlagBits mShaderStage;
    VkShaderModule mShaderModule;
    VkShaderModuleCreateInfo mCreateInfo;
    VkPipelineShaderStageCreateInfo mShaderStageInfo;
    VkDevice mDevice;

public:
    VulkanShader(const std::string &name, const std::string &path, VkStructureType shaderType, VkShaderStageFlagBits mShaderStage, VkDevice aDevice);

    VkShaderModuleCreateInfo &ShaderModuleCreateInfoRef() { return mCreateInfo; }
    VkPipelineShaderStageCreateInfo &ShaderStageInfoRef() { return mShaderStageInfo; };
    void DestroyShaderModule() { vkDestroyShaderModule(mDevice, mShaderModule, nullptr); }
};
