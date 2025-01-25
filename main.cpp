#include "Vulkan/VulkanPipeLine.hpp"
#include "Vulkan/VulkanApp.hpp"


int main()
{
    GPGVulkan::VulkanPipeLine appWindow(1920, 1080);
    GPGVulkan::VulkanApp app(appWindow);

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
