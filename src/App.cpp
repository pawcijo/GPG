
#include <App.h>

#include <iostream>
#include <Shader.h>

#include <memory>

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

void App::ProcessKey()
{
    if (glfwGetKey(mAppWindow.GetWindow(), GLFW_KEY_F1) == GLFW_PRESS && !F1_Pressed)
    {
        F1_Pressed = true;
        SwitchDrawMode();
    }
    else if (!(glfwGetKey(mAppWindow.GetWindow(), GLFW_KEY_F1) == GLFW_PRESS) &&
             F1_Pressed)
    {
        F1_Pressed = false;
    }
}

void App::SwitchDrawMode()
{

    if (mDrawMode == DrawMode::EDefault)
    {
        mDrawMode = DrawMode::EWireFrame;
        for (auto &square : mSquares)
        {
            square->SetDrawMode(DrawMode::EWireFrame);
        }
    }
    else
    {
        mDrawMode = DrawMode::EDefault;
        for (auto &square : mSquares)
        {
            square->SetDrawMode(DrawMode::EDefault);
        }
    }
}

App::App(AppWindow::AppWindow &appWindow) : mAppWindow(appWindow)
{
}

void App::Run()
{
    auto window = mAppWindow.GetWindow();

    std::unique_ptr<Shader> shader = std::make_unique<Shader>("shaders/shader.vs", "shaders/shader.fs");
    shader->use();

    mSquares.push_back(new Square());

    DrawMode drawMode = DrawMode::EDefault;

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        ProcessKey();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto shaderPtr = shader.get();
        for (Square *square : mSquares)
        {

            square->Draw(shaderPtr);
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    printf("Close App.\n");
    glfwTerminate();
}