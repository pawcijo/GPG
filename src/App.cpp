
#include <App.h>

#include <iostream>
#include <Shader.h>

#include <memory>

#include <filesystem>

#include "stb_image.h"

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

void App::ProcessKey()
{
    auto window = mAppWindow.GetWindow();
    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS && !F1_Pressed)
    {
        F1_Pressed = true;
        SwitchDrawMode();
    }
    else if (!(glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) &&
             F1_Pressed)
    {
        F1_Pressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS && !F2_Pressed)
    {
        F2_Pressed = true;
        SwitchCameraMode();
    }
    else if (!(glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) &&
             F2_Pressed)
    {
        F2_Pressed = false;
    }

    float speed = deltaTime * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.Position += speed * camera.Front;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.Position -= speed * camera.Front;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.Position -=
            glm::normalize(glm::cross(camera.Front, camera.Up)) * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.Position +=
            glm::normalize(glm::cross(camera.Front, camera.Up)) * speed;
    }
}

void App::ProcessMouse()
{
    if (processMouseMovement)
    {

        if (mouseCallBack)
        {
            if (mouseToUpdate)
            {
                mouseToUpdate = false;
                if (firstMouse)
                {
                    lastX = mousePosX;
                    lastY = mousePosY;
                    firstMouse = false;
                }

                float xoffset = mousePosX - lastX;
                float yoffset =
                    lastY - mousePosY; // reversed since y-coordinates go from bottom to top

                lastX = mousePosX;
                lastY = mousePosY;

                camera.ProcessMouseMovement(xoffset, yoffset);
            }
        }
    }
}

void App::SwitchCameraMode()
{
    if (Perspective == cameraMode)
    {
        cameraMode = Orthographic;
    }
    else
    {
        cameraMode = Perspective;
    }
}

void App::SwitchDrawMode()
{

    if (mDrawMode == DrawMode::EDefault)
    {
        mDrawMode = DrawMode::EWireFrame;
        for (auto &box : mBoxes)
        {
            box->SetDrawMode(DrawMode::EWireFrame);
        }
    }
    else
    {
        mDrawMode = DrawMode::EDefault;
        for (auto &box : mBoxes)
        {
            box->SetDrawMode(DrawMode::EDefault);
        }
    }
}

App::App(AppWindow::AppWindow &appWindow) : mAppWindow(appWindow),
                                            shader(std::make_unique<Shader>("shaders/shader.vs", "shaders/shader.fs")),
                                            shader_test(std::make_unique<Shader>("shaders/test.vs", "shaders/test.fs")),
                                            box_shader(std::make_unique<Shader>("shaders/boxShader.vs", "shaders/boxShader.fs"))
{

    glfwSetCursorPosCallback(mAppWindow.GetWindow(), cursor_position_callback);

    stbi_set_flip_vertically_on_load(true);
}

void App::Run()
{
    auto window = mAppWindow.GetWindow();

    mBoxes.push_back(new Box("resources/textures/clown.png", DrawMode::EDefault));
    mBoxes.push_back(new Box("resources/textures/clown_2.png", DrawMode::EDefault));

    mBoxes[0]->getTransform().translate(glm::vec3(2.0, 0, 0.0));

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        ProcessKey();
        ProcessMouse();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        SetViewAndPerspective(camera, *box_shader);

        auto shaderPtr = box_shader.get();
        for (Box *box : mBoxes)
        {

            box->Draw(box_shader.get());
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    printf("Close App.\n");
    glfwTerminate();
}

void App::SetViewAndPerspective(Camera &aCamera, Shader &aShader)
{
    if (cameraMode == Perspective)
    {
        projection = glm::perspective(
            aCamera.Zoom, (float)mAppWindow.GetWidth() / (float)mAppWindow.Getheight(),
            0.1f, 10000.0f);
    }
    else
    {
        projection = glm::ortho(
            (float)0.0, (float)mAppWindow.GetWidth() / (float)mAppWindow.Getheight(),
            0.1f, 10000.0f);
    }
    
    view = aCamera.GetViewMatrix();

    aShader.use();
    aShader.setMat4("projection", projection);
    aShader.setMat4("view", view);
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
{
    mouseToUpdate = true;
    mousePosX = xpos;
    mousePosY = ypos;
}
