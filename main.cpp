#include <Vulkan/VulkanPipeLine.h>
#include <Vulkan/VulkanApp.hpp>

#include <stdexcept>
//#include <print>

int main()
{
    VulkanPipeLine appWindow(1920, 1080);
    VulkanApp app(appWindow);

    try
    {
        app.Run();
    }
    catch (const std::exception &e)
    {
        printf("Exception %s \n.", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

    return 0;
}
