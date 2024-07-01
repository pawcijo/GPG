#include "AppVulkan.hpp"

#include <SDL2/SDL.h>

unsigned long GetApplicationTime()
{
    return SDL_GetTicks();
}

std::unique_ptr<PhysicsTimer> CreatePhysicsTimer(unsigned int alUpdatesPerSec)
{
    return std::make_unique<PhysicsTimer>(alUpdatesPerSec);
}

AppVulkan::AppVulkan(AppWindowVulkan::AppWindowVulkan &window) : mAppWindow(window)
{
    mPhysicsTimerPtr = CreatePhysicsTimer(60); // 1 physics update per second
    mFrametimestart = ((float)GetApplicationTime()) / 1000.0f;
}

void AppVulkan::Run()
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

                unsigned int lDeltaTime = GetApplicationTime() - lUpdateTime;
                mUpdateTime = (float)(lDeltaTime) / 1000.0f;

                mIsUpdated = true;
                mGameTime += mPhysicsTimerPtr->GetStepSize();
            }
            mPhysicsTimerPtr->EndUpdateLoop();
        }

        // Draw stuff
        {
            mAppWindow.DrawFrame();
        }
    }
    vkDeviceWaitIdle(mAppWindow.GetDevice());
    CleanUp();
}

void AppVulkan::CleanUp()
{
    mAppWindow.CleanUp();
    glfwTerminate();
}

AppVulkan::~AppVulkan() {}