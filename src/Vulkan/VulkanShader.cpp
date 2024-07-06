#include "VulkanShader.hpp"

#include <Vulkan/VulkanUtis.hpp>

namespace GPGVulkan
{

    VkShaderModule createShaderModule(const std::vector<char> &code, VkDevice aDevice)
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(aDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create shader module!");
        }
        else
        {
            printf("Shader module created sucessfully.\n");
        }

        return shaderModule;
    }

    VulkanShader::VulkanShader(const std::string &aName,
                               const std::string &aPath,
                               VkStructureType aShaderType,
                               VkShaderStageFlagBits aShaderStage,
                               VkDevice aDevice) : mName(aName),
                                                   mShaderType(aShaderType),
                                                   mShaderStage(aShaderStage),
                                                   mDevice(aDevice)
    {

        auto shaderCode = readFile(aPath);

        mShaderModule = createShaderModule(shaderCode, mDevice);

        mShaderStageInfo = VkPipelineShaderStageCreateInfo{};
        mShaderStageInfo.sType = aShaderType;
        mShaderStageInfo.stage = aShaderStage;
        mShaderStageInfo.module = mShaderModule;
        mShaderStageInfo.pName = "main";
    }

}