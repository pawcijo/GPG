#pragma once

#include "Vulkan/VulkanPipeLine.hpp"
#include "Vulkan/VulkanUtis.hpp"
#include "Common/Scene.hpp"
#include "MyPhysics/PhysicsTimer.h"

#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace GPGVulkan
{

    class Model;
    class VulkanApp
    {
        VulkanPipeLine &mGraphicPipeline;

        // Time related stuff
        bool mPause = false;
        std::unique_ptr<PhysicsTimer> mPhysicsTimerPtr;
        float mFrametimestart;
        float mUpdateTime;
        bool mIsUpdated;
        double mGameTime;

        // Camera related
        void ProcessCameraMovement();
        void SetViewAndPerspective();
        Camera mCamera;

        // Scene
        Scene mScene;
        std::vector<Model*> mModels;
        //move textures here ?



        // Clean up
        void CleanUp();

    public:
        VulkanApp(VulkanPipeLine &window);
        ~VulkanApp();
        void Run();

        friend class VulkanPipeline;
    };

}