#pragma once

#include "Vulkan/VulkanUtis.hpp"
#include "Common/Scene.hpp"
#include "Common/Camera.h"
#include "MyPhysics/PhysicsTimer.h"

#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace GPGVulkan
{

    class Model;
    class VulkanPipeLine;
    class VulkanApp
    {
    private:
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
        Scene* mScene;
        std::vector<Model *> mModels;
        // move textures here ?

        // Clean up
        void CleanUp();

    public:
        VulkanApp(VulkanPipeLine &window);
        ~VulkanApp();
        void Run();

        std::vector<Model *> &LoadedModels();
        Model *GetModel(std::filesystem::path aPath);
        long long GetModelsSizeInBytes();
        void AddModel(Model *);

        friend class VulkanPipeLine;
    };

}