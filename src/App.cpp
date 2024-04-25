

#include <App.h>

#include <iostream>
#include <Shader.h>

#include <memory>

#include <filesystem>

#include "stb_image.h"

#include <glm/gtc/quaternion.hpp>

#include <MyPhysics/Obb.h>

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

    float speed = deltaTime * mCamera.MovementSpeed;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        mCamera.Position += speed * mCamera.Front;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        mCamera.Position -= speed * mCamera.Front;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        mCamera.Position -=
            glm::normalize(glm::cross(mCamera.Front, mCamera.Up)) * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        mCamera.Position +=
            glm::normalize(glm::cross(mCamera.Front, mCamera.Up)) * speed;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        mCamera.Position += speed * mCamera.Up;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        mCamera.Position -= speed * mCamera.Up;
    }

    if (selecteObb)
    {
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {

            selecteObb->Translate(speed * glm::vec3(0.0f, 1.0f, 0.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            selecteObb->Translate((-1.0f * speed) * glm::vec3(0.0f, 1.0f, 0.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            selecteObb->Translate((-1.0f * speed) * glm::vec3(1.0f, 0.0f, 0.0f));
        }

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            selecteObb->Translate(speed * glm::vec3(1.0f, 0.0f, 0.0f));
        }
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
                // //printf("Offset x: %f offset Y %f \n", xoffset, yoffset);
                mCamera.ProcessMouseMovement(-1.0 * xoffset, -1.0 * yoffset);
            }
        }
    }
}

void App::SwitchCameraMode()
{
    if (Perspective == mCamera.GetCameraMode())
    {
        mCamera.SetCameraMode(Orthographic);
    }
    else
    {
        mCamera.SetCameraMode(Perspective);
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
                                            mCamera(glm::vec3(CAMERA_DEFAULT_POSTITION), glm::vec3(CAMERA_DEFAULT_WORLD_UP), 276, -25)
{
    auto window = appWindow.GetWindow();
    ImGui_ImplGlfw_RestoreCallbacks(window);

    stbi_set_flip_vertically_on_load(true);

    glfwSetCursorPosCallback(window, CursorPositonCallback);
    glfwSetMouseButtonCallback(window, MouseClickCallback);

    // Imgui Callbacks
    glfwSetWindowFocusCallback(window, ImGui_ImplGlfw_WindowFocusCallback);
    glfwSetCursorEnterCallback(window, ImGui_ImplGlfw_CursorEnterCallback);
    glfwSetScrollCallback(window, ImGui_ImplGlfw_ScrollCallback);
    glfwSetKeyCallback(window, ImGui_ImplGlfw_KeyCallback);
    glfwSetCharCallback(window, ImGui_ImplGlfw_CharCallback);
    glfwSetMonitorCallback(ImGui_ImplGlfw_MonitorCallback);

    pm = new PhysicsManager(0.0f, -9.8f, 0.0f);
    pm->SetApp(this);

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

    mBoxes[0]->getTransform().translate(glm::vec3(2.0, 0, 0.0));

    mBoxes[2]->getTransform().setPosition(glm::vec3(0, -1.0f, 0));
    mBoxes[2]->getTransform().setScale(50, 1, 50);

    pm->AddOBB(new Obb(mBoxes[0]->getTransform(), 2.0f, glm::vec3(0.0f, 0.0f, 0.0f)));
    pm->AddOBB(new Obb(mBoxes[1]->getTransform(), 2.0f, glm::vec3(0.0f, 0.0f, 0.0f)));
    pm->AddOBB(new Obb(mBoxes[2]->getTransform(), 2.0f, glm::vec3(0.0f, 0.0f, 0.0f), true));

    for (int i = 0; i < mBoxes.size(); i++)
    {
        pm->obbs[i]->object = mBoxes[i];
    }

    mBoxes[0]->SetObb(pm->obbs[0]);
    mBoxes[1]->SetObb(pm->obbs[1]);
    mBoxes[2]->SetObb(pm->obbs[2]);
}

void App::Run()
{
    auto window = mAppWindow.GetWindow();

    int counter = 0;
    bool prawda = true;
    ImGuiIO &io = ImGui::GetIO();
    while (!glfwWindowShouldClose(window))
    {

        // printf("Render \n");
        processInput(window);
        ProcessKey();
        ProcessMouse();

        // TODO make deltaTime work more robust
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (!pause)
        {
            pm->Update(deltaTime);
        }

        pm->SynchroniseWithRender();

        SetViewAndPerspective(mCamera);

        if (!io.WantCaptureMouse)
        {
            ColorPicking();
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto shaderPtr = box_shader.get();
        for (Box *box : mBoxes)
        {

            box->Draw(box_shader.get(), this);
        }

        ImGuiStuff();

        pm->ClearCollisonMap();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(NULL);

    // printf("Close App.\n");
    glfwDestroyWindow(window);
    glfwTerminate();
}

void App::ImGuiStuff()
{

    // New frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Koks okienko");
    if (ImGui::CollapsingHeader("Orthograpic"))
    {
        ImGui::SliderFloat("Left", &mCamera.orthographicSettings.left, -100.0f, 100.0f);
        ImGui::SliderFloat("Right", &mCamera.orthographicSettings.right, -100.0f, 100.0f);
        ImGui::SliderFloat("Bottom", &mCamera.orthographicSettings.bottom, -100.0f, 100.0f);
        ImGui::SliderFloat("Top", &mCamera.orthographicSettings.top, -100.0f, 100.0f);
        ImGui::SliderFloat("Near", &mCamera.orthographicSettings.zNear, -100.0f, 100.0f);
        ImGui::SliderFloat("Far", &mCamera.orthographicSettings.zFar, -100.0f, 200.0f);
    }

    if (ImGui::CollapsingHeader("Perspective"))
    {
        ImGui::SliderFloat("Fov", &mCamera.Zoom, 0.0f, 90.0f);
    }

    ImGui::Text("Physics object 1: [%f][%f][%f] ",
                pm->obbs[0]->GetCenter().x,
                pm->obbs[0]->GetCenter().y,
                pm->obbs[0]->GetCenter().z);

    ImGui::Text("Physics object 2: [%f][%f][%f] ",
                pm->obbs[1]->GetCenter().x,
                pm->obbs[1]->GetCenter().y,
                pm->obbs[1]->GetCenter().z);

    ImGui::Text("Ground box object posiiton: [%f][%f][%f] ",
                pm->obbs[2]->GetCenter().x,
                pm->obbs[2]->GetCenter().y,
                pm->obbs[2]->GetCenter().z);

    if (ImGui::CollapsingHeader("Collision", ImGuiTreeNodeFlags_DefaultOpen))
    {

        if (ImGui::Button("Pause/Resume"))
        {
            pause = !pause;
        }

        if (ImGui::Button("Resolve all collison"))
        {
            pm->ResolveAllCollision();
        }

        for (auto collisionPair : pm->collisionSet)
        {
            ImGui::Text("%i %i", collisionPair.first->object->ObjectId(), collisionPair.second->object->ObjectId());
        }
    }

    if (ImGui::CollapsingHeader("Selected Object", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Selected object %i", selectedObject);
        if (nullptr != selecteObb)
        {
            glm::vec3 rotation = selecteObb->GetRotation();
            ImGui::Text("Rotation  X:%f Y:%f Z:%f ", rotation.x, rotation.y, rotation.z);
        }

        if (ImGui::Button("Rotate X +"))
        {

            if (nullptr != selecteObb)
                selecteObb->Rotate(15.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        }
        if (ImGui::Button("Rotate X -"))
        {
            if (nullptr != selecteObb)
                selecteObb->Rotate(-15.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        }
        if (ImGui::Button("Rotate Y +"))
        {
            if (nullptr != selecteObb)
                selecteObb->Rotate(15.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        }
        if (ImGui::Button("Rotate Y -"))
        {
            if (nullptr != selecteObb)
                selecteObb->Rotate(-15.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        }
        if (ImGui::Button("Rotate Z +"))
        {
            if (nullptr != selecteObb)
                selecteObb->Rotate(15.0f, glm::vec3(0.0f, 0.0f, 1.0f));
        }
        if (ImGui::Button("Rotate z -"))
        {
            if (nullptr != selecteObb)
                selecteObb->Rotate(-15.0f, glm::vec3(0.0f, 0.0f, 1.0f));
        }
    }

    ImGui::End();

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void App::ColorPicking()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (int i = 0; i < mBoxes.size(); i++)
    {

        mBoxes[i]->Draw_Color(color_pick_shader.get(), mAppWindow);
    }

    glFlush();
    glFinish();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    if (Left_Mouse_click && mCanPick)
    {
        unsigned char data[4];

        // Assuming screen starts from left and window is somewhere up the ground
        glReadPixels(mousePosX, mAppWindow.Getheight() - mousePosY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

        int pickedID =
            data[0] +
            data[1] * 256 +
            data[2] * 256 * 256;

        selectedObject = pickedID;
        if (pickedID < pm->obbs.size())
        {
            selecteObb = pm->obbs[selectedObject];
        }
        else
        {
            selecteObb = nullptr;
        }
        mCanPick = false;
    }
}

void App::SetViewAndPerspective(Camera &aCamera)
{
    if (aCamera.GetCameraMode() == Perspective)
    {
        aCamera.mProjection = glm::perspective(
            glm::radians(aCamera.Zoom), (float)mAppWindow.GetWidth() / (float)mAppWindow.Getheight(),
            0.1f, 10000.0f);
    }
    else
    {
        aCamera.mProjection = glm::ortho((double)mCamera.orthographicSettings.left,
                                         (double)mCamera.orthographicSettings.right,
                                         (double)mCamera.orthographicSettings.bottom,
                                         (double)mCamera.orthographicSettings.top,
                                         (double)mCamera.orthographicSettings.zNear,
                                         (double)mCamera.orthographicSettings.zFar);
    }

    auto view = aCamera.GetViewMatrix();

    box_shader->use();
    box_shader->setMat4("projection", aCamera.mProjection);
    box_shader->setMat4("view", view);

    color_pick_shader->use();
    color_pick_shader->setMat4("projection", aCamera.mProjection);
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

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
    {
        Left_Mouse_click = true;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE)
    {
        Left_Mouse_click = false;
        mCanPick = true;
    }
}

App::~App()
{
    // printf("App destroyed \n");
}