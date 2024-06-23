#include "AppVulkan.hpp"

AppVulkan::AppVulkan(AppWindowVulkan::AppWindowVulkan &window) : mAppWindow(window) {}

void AppVulkan::Run()
{

    while (!glfwWindowShouldClose(mAppWindow.GetWindow()))
    {
        glfwPollEvents();
    }

    CleanUp();
}

void AppVulkan::CleanUp()
{
    vkDestroyInstance(mAppWindow.GetInstance(), nullptr);
    glfwDestroyWindow(mAppWindow.GetWindow());
    glfwTerminate();
}

AppVulkan::~AppVulkan() {}