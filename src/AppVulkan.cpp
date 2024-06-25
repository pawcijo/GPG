#include "AppVulkan.hpp"

AppVulkan::AppVulkan(AppWindowVulkan::AppWindowVulkan &window) : mAppWindow(window) {}

void AppVulkan::Run()
{
    while (!glfwWindowShouldClose(mAppWindow.GetWindow()))
    {
        glfwPollEvents();
        mAppWindow.DrawFrame();
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