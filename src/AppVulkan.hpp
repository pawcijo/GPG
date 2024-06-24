#pragma once

#include <AppWindowVulkan.h>


#include <glm/glm.hpp>

#include <vector>
#include <memory>

#include <Vulkan/VulkanUtis.hpp>

class AppVulkan
{

     AppWindowVulkan::AppWindowVulkan &mAppWindow;
     void CleanUp();
public:
    AppVulkan(AppWindowVulkan::AppWindowVulkan &window);
    ~AppVulkan(); 
    void Run();

};