#include <AppWindowVulkan.h>
#include <AppVulkan.hpp>

#include <stdexcept>
#include <print>

int main()
{
    AppWindowVulkan::AppWindowVulkan appWindow(1920, 1080);
    AppVulkan app(appWindow);

    try
    {
        app.Run();
    }
    catch (const std::exception &e)
    {
        std::println("Exception {}", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

    return 0;
}
