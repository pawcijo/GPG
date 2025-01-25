#include "VulkanApp.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include "Model.hpp"

#include "Physics/PhysicsEntityManager.hpp"

#include <SDL2/SDL.h>

namespace GPGVulkan
{
    unsigned long GetApplicationTime()
    {
        return SDL_GetTicks();
    }

    std::unique_ptr<PhysicsTimer> CreatePhysicsTimer(unsigned int alUpdatesPerSec)
    {
        return std::make_unique<PhysicsTimer>(alUpdatesPerSec);
    }

    VulkanApp::VulkanApp(VulkanPipeLine &aGraphicPipeline)
        : mGraphicPipeline(aGraphicPipeline), mScene(nullptr), mCamera(Camera())
    {
        mPhysicsTimerPtr = CreatePhysicsTimer(60); // 1 physics update per second
        mFrametimestart = ((float)GetApplicationTime()) / 1000.0f;

        mGraphicPipeline.SetScenePtr(mScene);
        mGraphicPipeline.SetAppPtr(this);
    }

    void VulkanApp::SetViewAndPerspective()
    {
        if (mCamera.GetCameraMode() == Perspective)
        {
            mCamera.mProjection = glm::perspective(glm::radians(mCamera.Zoom),
                                                   (float)mGraphicPipeline.GetWidth() /
                                                       (float)mGraphicPipeline.Getheight(),
                                                   0.1f, 10.0f);
        }
        else
        {
            mCamera.mProjection =
                glm::ortho((double)mCamera.orthographicSettings.left,
                           (double)mCamera.orthographicSettings.right,
                           (double)mCamera.orthographicSettings.bottom,
                           (double)mCamera.orthographicSettings.top,
                           (double)mCamera.orthographicSettings.zNear,
                           (double)mCamera.orthographicSettings.zFar);
        }

        mCamera.updateCameraVectors();
    }

    void VulkanApp::ProcessCameraMovement()
    {

        float speed = mPhysicsTimerPtr->GetStepSize() * mCamera.MovementSpeed;
        auto window = mGraphicPipeline.GetWindow();
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            mCamera.Position += speed * mCamera.Front;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            mCamera.Position -= speed * mCamera.Front;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            mCamera.Position -=
                glm::normalize(glm::cross(mCamera.Front, mCamera.Up)) * speed;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            mCamera.Position +=
                glm::normalize(glm::cross(mCamera.Front, mCamera.Up)) * speed;
        }

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            mCamera.Position += speed * mCamera.Up;
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            mCamera.Position -= speed * mCamera.Up;
        }
    }

    Model *VulkanApp::GetModel(std::filesystem::path aPath)
    {
        for (auto model : mModels)
        {
            if (model->ModelPath() == aPath)
            {
                return model;
            }
        }
        return nullptr;
    }

    void VulkanApp::AddModel(Model *aModel)
    {
        for (Model *model : mModels)
        {
            if (model->ModelPath() == aModel->ModelPath())
            {
                model->mTransform = aModel->mTransform;
                printf("Model already exist, skipping .\n");
                return;
            }
        }
        mModels.push_back(aModel);
    }

    bool VulkanApp::CheckIfModelExist(std::filesystem::path aPath)
    {
        for (Model *model : mModels)
        {
            if (model->ModelPath() == aPath)
            {
                return true;
            }
        }
        return false;
    }

    void VulkanApp::Run()
    {
        // move them to model  mGraphicPipeline.createDescriptorSets();

        mGraphicPipeline.createDescriptorPool(1000);
        mGraphicPipeline.createUniformBuffers();

        mGraphicPipeline.createCommandBuffers();
        mGraphicPipeline.createSyncObjects();
        mGraphicPipeline.setupImgui();
       

        while (!glfwWindowShouldClose(mGraphicPipeline.GetWindow()))
        {
            glfwPollEvents();

            // Physics stuff
            {
                while (mPhysicsTimerPtr->WantUpdate() && !mPause)
                {
                    unsigned int lUpdateTime = GetApplicationTime();

                    // Update physcis here
                    // PhysicsUpdate(mPhysicsTimerPtr->GetStepSize());

                    // Update Camera here or in draw section ?
                    SetViewAndPerspective();
                    ProcessCameraMovement();

                    unsigned int lDeltaTime = GetApplicationTime() - lUpdateTime;
                    mUpdateTime = (float)(lDeltaTime) / 1000.0f;

                    mIsUpdated = true;
                    mGameTime += mPhysicsTimerPtr->GetStepSize();
                }
                mPhysicsTimerPtr->EndUpdateLoop();
            }

            // Draw stuff
            {
                mGraphicPipeline.DrawFrame(mCamera);
            }
        }
        vkDeviceWaitIdle(mGraphicPipeline.GetDevice());
        CleanUp();
    }

    long long VulkanApp::GetModelsSizeInBytes()
    {
        long long wholeSize = 0;

        for (auto model : mModels)
        {
            wholeSize += model->GetModelSizeInBytes();
        }

        return wholeSize;
    }

    std::vector<Model *> &VulkanApp::LoadedModels()
    {
        return mModels;
    }

    void VulkanApp::CleanUp()
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        mGraphicPipeline.CleanUp();
        glfwTerminate();
    }

    VulkanApp::~VulkanApp() {}

}