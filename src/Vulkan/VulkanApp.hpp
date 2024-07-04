#pragma once

#include "Vulkan/VulkanPipeLine.h"
#include "Vulkan/VulkanUtis.hpp"
#include "MyPhysics/PhysicsTimer.h"

#include <glm/glm.hpp>

#include <vector>
#include <memory>

class VulkanApp
{
    VulkanPipeLine &mAppWindow;

    // Time related stuff
    bool mPause = false;
    std::unique_ptr<PhysicsTimer> mPhysicsTimerPtr;
    float mFrametimestart;
    float mUpdateTime;
    bool mIsUpdated;
    double mGameTime;

    //Camera related
    void ProcessCameraMovement();
    void SetViewAndPerspective();
    Camera mCamera;

    //Clean up
    void CleanUp();

public:
    VulkanApp(VulkanPipeLine &window);
    ~VulkanApp();
    void Run();
};