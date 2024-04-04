

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

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        camera.Position += speed * camera.Up;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        camera.Position -= speed * camera.Up;
    }
}

void App::ProcessMouse()
{

    if (processMouseMovement)
    {

        if (Middle_Mouse_Hold)
        {
            mouseToUpdate = false;
            if (firstMouse)
            {
                lastX = mousePosX;
                lastY = mousePosY;
                firstMouse = false;
            }

            float xoffset = mousePosX - initialMousePosX;
            // reversed since y-coordinates go from bottom to top
            float yoffset = initialMousePosY - mousePosY;

            float xoffset_2 = mousePosX - mousePosX;
            // reversed since y-coordinates go from bottom to top
            float yoffset_2 = mousePosY - mousePosY;

            lastX = mousePosX;
            lastY = mousePosY;

            // if (abs(xoffset_2) > 0.1 || abs(yoffset_2) > 0.1)
            {
                // printf("Offset x: %f offset Y %f \n", xoffset, yoffset);
                camera.ProcessMouseMovement(-1.0 * xoffset, -1.0 * yoffset);
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
                                            box_shader(std::make_unique<Shader>("shaders/boxShader.vs", "shaders/boxShader.fs")),
                                            color_pick_shader(std::make_unique<Shader>("shaders/colorPick.vs", "shaders/colorPick.fs")),
                                            camera(glm::vec3(CAMERA_DEFAULT_POSTITION), glm::vec3(CAMERA_DEFAULT_WORLD_UP), 276, -25)
{
    auto window = appWindow.GetWindow();
    ImGui_ImplGlfw_RestoreCallbacks(window);

    stbi_set_flip_vertically_on_load(true);

    glfwSetCursorPosCallback(window, CursorPositonCallback);
    glfwSetMouseButtonCallback(window, MouseClickCallback);

    glfwSetWindowFocusCallback(window, ImGui_ImplGlfw_WindowFocusCallback);
    glfwSetCursorEnterCallback(window, ImGui_ImplGlfw_CursorEnterCallback);
    glfwSetScrollCallback(window, ImGui_ImplGlfw_ScrollCallback);
    glfwSetKeyCallback(window, ImGui_ImplGlfw_KeyCallback);
    glfwSetCharCallback(window, ImGui_ImplGlfw_CharCallback);
    glfwSetMonitorCallback(ImGui_ImplGlfw_MonitorCallback);
}

void App::Run()
{
    auto window = mAppWindow.GetWindow();

    mBoxes.push_back(new Box("resources/textures/clown.png",
                             DrawMode::EDefault,
                             box_shader.get(),
                             color_pick_shader.get(), 0));
    mBoxes.push_back(new Box("resources/textures/clown_2.png",
                             DrawMode::EDefault,
                             box_shader.get(),
                             color_pick_shader.get(), 1));
    mBoxes.push_back(new Box("resources/textures/box.jpg",
                             DrawMode::EDefault,
                             box_shader.get(),
                             color_pick_shader.get(), 2));

    mBoxes.push_back(new Box("resources/textures/juanP.jpg",
                             DrawMode::EDefault,
                             box_shader.get(),
                             color_pick_shader.get(), 3));

    mBoxes[0]->getTransform().translate(glm::vec3(2.0, 0, 0.0));
    mBoxes[3]->getTransform().translate(glm::vec3(4.0, 0, 0.0));

    mBoxes[2]->getTransform().translate(glm::vec3(0.0f, -0.55f, 0.0f));
    mBoxes[2]->getTransform().setScale(10.0f, 10.0f, 10.0f);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    int counter = 0;
    bool prawda = true;
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        ProcessKey();
        ProcessMouse();

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        SetViewAndPerspective(camera);

        for (int i = 0; i < mBoxes.size(); i++)
        {

            mBoxes[i]->Draw_Color(color_pick_shader.get(), mAppWindow);
        }

        glFlush();
        glFinish();
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
        {
            unsigned char data[4];
            glReadPixels(mousePosX, mAppWindow.Getheight() - mousePosY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

            int pickedID =
                data[0] +
                data[1] * 256 +
                data[2] * 256 * 256;

            selectedObject = pickedID;
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto shaderPtr = box_shader.get();
        for (Box *box : mBoxes)
        {

            box->Draw(box_shader.get(),this);
        }

        {
            // New frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("Camera Settings");
            if (ImGui::CollapsingHeader("Orthograpic"))
            {
                ImGui::SliderFloat("Left", &camera.orthographicSettings.left, -100.0f, 100.0f);
                ImGui::SliderFloat("Right", &camera.orthographicSettings.right, -100.0f, 100.0f);
                ImGui::SliderFloat("Bottom", &camera.orthographicSettings.bottom, -100.0f, 100.0f);
                ImGui::SliderFloat("Top", &camera.orthographicSettings.top, -100.0f, 100.0f);
                ImGui::SliderFloat("Near", &camera.orthographicSettings.zNear, -100.0f, 100.0f);
                ImGui::SliderFloat("Far", &camera.orthographicSettings.zFar, -100.0f, 200.0f);
            }

            if (ImGui::CollapsingHeader("Perspective"))
            {

                ImGui::SliderFloat("Fov", &camera.Zoom, 0.0f, 90.0f);
            }
            ImGui::Text("Selected object %i", selectedObject);

            ImGui::End();

            ImGui::Render();

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    printf("Close App.\n");
    glfwDestroyWindow(window);
    glfwTerminate();
}

void App::SetViewAndPerspective(Camera &aCamera)
{
    if (cameraMode == Perspective)
    {
        projection = glm::perspective(
            glm::radians(aCamera.Zoom), (float)mAppWindow.GetWidth() / (float)mAppWindow.Getheight(),
            0.1f, 10000.0f);
    }
    else
    {

        projection = glm::ortho((double)camera.orthographicSettings.left,
                                (double)camera.orthographicSettings.right,
                                (double)camera.orthographicSettings.bottom,
                                (double)camera.orthographicSettings.top,
                                (double)camera.orthographicSettings.zNear,
                                (double)camera.orthographicSettings.zFar);
    }

    view = aCamera.GetViewMatrix();

    box_shader->use();
    box_shader->setMat4("projection", projection);
    box_shader->setMat4("view", view);

    color_pick_shader->use();
    color_pick_shader->setMat4("projection", projection);
    color_pick_shader->setMat4("view", view);
}

void CursorPositonCallback(GLFWwindow *window, double xpos, double ypos)
{

    ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
    mousePosX = xpos;
    mousePosY = ypos;
}

void MouseClickCallback(GLFWwindow *window, int button, int action, int mods)
{

    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_3) == GLFW_PRESS)
    {

        Middle_Mouse_Hold = true;
        glfwGetCursorPos(window, &initialMousePosX, &initialMousePosY);
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_3) == GLFW_RELEASE)
    {
        Middle_Mouse_Hold = false;
    }
}

App::~App()
{
    printf("App destroyed \n");
}