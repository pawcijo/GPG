#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <AppWindow.h>
#include <SimpleShape/Box.h>
#include <Camera.h>

/*
#include <Physics/PhysicsManager.h>
#include <Physics/RigidbodyImpl.h>
*/

#include <MyPhysics/PhysicsManager.h>

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
static bool Left_Mouse_click = false;
static bool Middle_Mouse_Hold = false;

// Color Picking
static bool mCanPick = true;

static glm::vec3 globalCameraPosition;
static float globalCameraYaw;
static float globalCameraPitch;

void CursorPositonCallback(GLFWwindow *window, double xpos, double ypos);
void MouseClickCallback(GLFWwindow *window, int button, int action, int mods);

class App
{
    // Shaders
    std::unique_ptr<Shader> shader;
    std::unique_ptr<Shader> shader_test;
    std::unique_ptr<Shader> box_shader;
    std::unique_ptr<Shader> color_pick_shader;

    // Render options
    DrawMode mDrawMode = DrawMode::EDefault;

    // Window
    AppWindow::AppWindow &mAppWindow;

    // Time related
    float deltaTime = 0.0f; //
    float lastFrame = 0.0f;

    // KEYS
    bool F1_Pressed = false;
    bool F2_Pressed = false;

    // Camera
    Camera mCamera;
    bool firstMouse = true;
    double lastY;
    double lastX;

    // App objects
    // TODO AppObject class
    std::vector<Box *> mBoxes;
    int selectedObject = -1;

    // Phsycis

    /* Old Physics
    PhysicsManager physicsManager;
    std::vector<RigidbodyImpl*> bodies;
    RigidbodyImpl* groundBox;
    void ResetPhyscis();
    */

    // My Gpt try

    PhysicsManager *pm = nullptr; // Gravity pointing down


    void SetViewAndPerspective(Camera &aCamera);
    void SwitchDrawMode();
    void SwitchCameraMode();

    void ProcessKey();
    void ProcessMouse();

public:
    App(AppWindow::AppWindow &window);
    ~App();
    void Run();

    friend class Box;
};