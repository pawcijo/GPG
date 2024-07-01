#pragma once

#include <AppWindowVulkan.h>

#include <glm/glm.hpp>

#include <vector>
#include <memory>

#include <Vulkan/VulkanUtis.hpp>

#include <MyPhysics/PhysicsTimer.h>

class AppVulkan
{
    AppWindowVulkan::AppWindowVulkan &mAppWindow;

    //Time related stuff
    bool mPause = false;
    std::unique_ptr<PhysicsTimer> mPhysicsTimerPtr;
    float mFrametimestart;
    float mUpdateTime;
    bool mIsUpdated;
    double mGameTime;

    void CleanUp();

public:
    AppVulkan(AppWindowVulkan::AppWindowVulkan &window);
    ~AppVulkan();
    void Run();
};