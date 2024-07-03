#include "VulkanApp.hpp"

#include <SDL2/SDL.h>

unsigned long GetApplicationTime()
{
    return SDL_GetTicks();
}

std::unique_ptr<PhysicsTimer> CreatePhysicsTimer(unsigned int alUpdatesPerSec)
{
    return std::make_unique<PhysicsTimer>(alUpdatesPerSec);
}

VulkanApp::VulkanApp(VulkanPipeLine &window) : mAppWindow(window), mCamera(Camera())
{
    mPhysicsTimerPtr = CreatePhysicsTimer(60); // 1 physics update per second
    mFrametimestart = ((float)GetApplicationTime()) / 1000.0f;
}

void VulkanApp::SetViewAndPerspective()
{
    if (mCamera.GetCameraMode() == Perspective)
    {
        mCamera.mProjection = glm::perspective(glm::radians(mCamera.Zoom),
                                               (float)mAppWindow.GetWidth() /
                                                   (float)mAppWindow.Getheight(),
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

void VulkanApp::CameraTest()
{

    float speed = mPhysicsTimerPtr->GetStepSize() * mCamera.MovementSpeed;
    auto window = mAppWindow.GetWindow();
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
    while (!glfwWindowShouldClose(mAppWindow.GetWindow()))
    {
        glfwPollEvents();

        // Physics stuff
        {
            while (mPhysicsTimerPtr->WantUpdate() && !mPause)
            {
                unsigned int lUpdateTime = GetApplicationTime();

                // Update physcis here
                // PhysicsUpdate(mPhysicsTimerPtr->GetStepSize());

                // Update Camera ? here
                SetViewAndPerspective();

                CameraTest();

                unsigned int lDeltaTime = GetApplicationTime() - lUpdateTime;
                mUpdateTime = (float)(lDeltaTime) / 1000.0f;

                mIsUpdated = true;
                mGameTime += mPhysicsTimerPtr->GetStepSize();
            }
            mPhysicsTimerPtr->EndUpdateLoop();
        }

        // Draw stuff
        {
            mAppWindow.DrawFrame(mCamera);
        }
    }
    vkDeviceWaitIdle(mAppWindow.GetDevice());
    CleanUp();
}

void VulkanApp::CleanUp()
{
    mAppWindow.CleanUp();
    glfwTerminate();
}

VulkanApp::~VulkanApp() {}