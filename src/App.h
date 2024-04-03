#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


#include <AppWindow.h>
#include <SimpleShape/Box.h>
#include <Camera.h>

#include <vector>
#include <memory>

// MOUSE
static bool mouseToUpdate = false;
static bool processMouseMovement = true;
static double mousePosX;
static double mousePosY;

static double initialMousePosX = 0.0;
static double initialMousePosY = 0.0;

// MOUSE BUTTONS
static bool Middle_Mouse_Hold = false;

static glm::vec3 globalCameraPosition;
static float globalCameraYaw;
static float globalCameraPitch;

void CursorPositonCallback(GLFWwindow* window, double xpos, double ypos);
void MouseClickCallback(GLFWwindow* window, int button, int action, int mods);



class App
{
    // Shaders
    std::unique_ptr<Shader> shader;
    std::unique_ptr<Shader> shader_test;
    std::unique_ptr<Shader> box_shader;

    // Time related
    float deltaTime = 0.0f; //
    float lastFrame = 0.0f;

    // KEYS
    bool F1_Pressed = false;
    bool F2_Pressed = false;

    // Camera
    Camera camera;
    glm::mat4 view;
    glm::mat4 projection;
    bool firstMouse = true;
    double lastY;
    double lastX;

    float cameraSpeed = 3;

    CameraMode cameraMode = Perspective;

    void SetViewAndPerspective(Camera &aCamera, Shader &aShader);

    AppWindow::AppWindow &mAppWindow;
    std::vector<Box *> mBoxes;
    DrawMode mDrawMode = DrawMode::EDefault;

    void SwitchDrawMode();
    void SwitchCameraMode();

    void ProcessKey();
    void ProcessMouse();

public:
    App(AppWindow::AppWindow &window);
    void Run();
};