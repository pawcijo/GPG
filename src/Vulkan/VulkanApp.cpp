#include "VulkanApp.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include "Model.hpp"

#include <SDL2/SDL.h>

unsigned long GetApplicationTime()
{
    return SDL_GetTicks();
}

std::unique_ptr<PhysicsTimer> CreatePhysicsTimer(unsigned int alUpdatesPerSec)
{
    return std::make_unique<PhysicsTimer>(alUpdatesPerSec);
}

namespace GPGVulkan
{

    VulkanApp::VulkanApp(VulkanPipeLine &aGraphicPipeline)
        : mGraphicPipeline(aGraphicPipeline), mCamera(Camera())
    {
        mPhysicsTimerPtr = CreatePhysicsTimer(60); // 1 physics update per second
        mFrametimestart = ((float)GetApplicationTime()) / 1000.0f;
        mGraphicPipeline.SetScene(&mScene);
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

    void VulkanApp::Run()
    {
        // Move to config  ?
        // TODO: do not load anything here, do it only if it is required
        const std::string MODEL_PATH = "resources/models/viking_room.obj";
        const std::string TEXTURE_PATH = "resources/textures/viking_room.png";

        const std::string MODEL_PATH_2 = "resources/models/Klocek.obj";
        const std::string TEXTURE_PATH_2 = "resources/textures/juanP.jpg";

        mGraphicPipeline.createDescriptorPool(2);
        mGraphicPipeline.createUniformBuffers();

        std::unique_ptr<Model> model = std::make_unique<Model>(MODEL_PATH, TEXTURE_PATH,
                                                               mGraphicPipeline.mVulkanContext);

        std::unique_ptr<Model> model_2 = std::make_unique<Model>(MODEL_PATH_2, TEXTURE_PATH_2,
                                                                  mGraphicPipeline.mVulkanContext);

        model->GetTransform().rotate(90, glm::vec3(1, 0, 0));
        model->GetTransform().rotate(180, glm::vec3(0, 1, 0));
        model->GetTransform().rotate(90, glm::vec3(0, 0, 1));

        SceneObject *scenObj = new SceneObject(Transform(), nullptr);
        scenObj->SetName("Dupa");
        scenObj->SetModel(std::move(model.get()));
        mScene.AddSceneObject(std::move(scenObj));

        SceneObject *emptySceneObj = new SceneObject(Transform(), nullptr);
        emptySceneObj->SetName("EmptySceneObj");
        scenObj->AddChild(std::move(emptySceneObj));

        SceneObject *emptySceneObj2 = new SceneObject(Transform(), nullptr);
        emptySceneObj2->SetModel(std::move(model_2.get()));
        emptySceneObj2->SetName("Klocek");

        model_2->GetTransform().setScale(glm::vec3(0.1, 0.1, 0.1));
        model_2->GetTransform().translate(glm::vec3(9, 0, 0));

        mScene.AddSceneObject(std::move(emptySceneObj2));

        mGraphicPipeline.SetScene(&mScene);

        // move them to model  mGraphicPipeline.createDescriptorSets();
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