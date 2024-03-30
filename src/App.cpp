
#include <App.h>

#include <iostream>

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

App::App(AppWindow::AppWindow& appWindow):mAppWindow(appWindow)
{

}

void App::Run()
{
    auto window = mAppWindow.GetWindow();
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    printf("Close App.\n");
    glfwTerminate();

}