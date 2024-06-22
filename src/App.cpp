

#include <App.h>

#include <Shader.h>
#include <iostream>

#include <memory>

#include <filesystem>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <GameObject/GameObjectBuilder.h>
#include <GameObject/GameObject.h>

#include <Mesh/Mesh.h>
#include <Mesh/ExampleMesh.h>

#include "SDL.h"

std::unique_ptr<PhysicsTimer> CreatePhysicsTimer(unsigned int alUpdatesPerSec)
{
    return std::make_unique<PhysicsTimer>(alUpdatesPerSec);
}

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
  else if (!(glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) && F1_Pressed)
  {
    F1_Pressed = false;
  }

  if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS && !F2_Pressed)
  {
    F2_Pressed = true;
    SwitchCameraMode();
  }
  else if (!(glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) && F2_Pressed)
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
        mCamera.ProcessMouseMovement(-1.0 * deltaTime * xoffset, -1.0 * deltaTime * yoffset);
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
    for (auto &object : manager.objectList)
    {
      object->GetMesh()->SetDrawMode(DrawMode::EWireFrame);
    }
  }
  else
  {
    mDrawMode = DrawMode::EDefault;
    for (auto &object : manager.objectList)
    {
      object->GetMesh()->SetDrawMode(DrawMode::EDefault);
    }
  }
}

App::App(AppWindow::AppWindow &appWindow)
    : mAppWindow(appWindow), shader(std::make_unique<Shader>(
                                 "shaders/shader.vs", "shaders/shader.fs")),
      shader_test(
          std::make_unique<Shader>("shaders/test.vs", "shaders/test.fs")),
      box_shader(std::make_unique<Shader>("shaders/boxShader.vs",
                                          "shaders/boxShader.fs")),
      color_pick_shader(std::make_unique<Shader>("shaders/colorPick.vs",
                                                 "shaders/colorPick.fs")),
      mCamera(glm::vec3(CAMERA_DEFAULT_POSTITION),
              glm::vec3(CAMERA_DEFAULT_WORLD_UP), 276, -25),
      scene(new PhysicsScene(dt))
{
  auto window = appWindow.GetWindow();
  ImGui_ImplGlfw_RestoreCallbacks(window);

  glfwSetCursorPosCallback(window, CursorPositonCallback);
  glfwSetMouseButtonCallback(window, MouseClickCallback);

  ImGui::GetIO().FontGlobalScale = 3.0;
  // Imgui Callbacks
  glfwSetWindowFocusCallback(window, ImGui_ImplGlfw_WindowFocusCallback);
  glfwSetCursorEnterCallback(window, ImGui_ImplGlfw_CursorEnterCallback);
  glfwSetScrollCallback(window, ImGui_ImplGlfw_ScrollCallback);
  glfwSetKeyCallback(window, ImGui_ImplGlfw_KeyCallback);
  glfwSetCharCallback(window, ImGui_ImplGlfw_CharCallback);
  glfwSetMonitorCallback(ImGui_ImplGlfw_MonitorCallback);

  // 60 updates per second for physics
  mPhysicsTimerPtr = CreatePhysicsTimer(60);

  scene->SetAllowSleep(true);
  scene->SetEnableFriction(true);
  scene->SetIterations(5);

  mFrametimestart = ((float)GetApplicationTime()) / 1000.0f;
}

void App::CreateGameObjects()
{

  mGameTime = 0;
  // Create the floor
  PhysicsBodyDef bodyDef;
  // bodyDef.axis.Set( q3RandomFloat( -1.0f, 1.0f ), q3RandomFloat( -1.0f, 1.0f
  // ), q3RandomFloat( -1.0f, 1.0f ) ); bodyDef.angle = q3PI * q3RandomFloat(
  // -1.0f, 1.0f );
  PhysicsBody *body = scene->CreateBody(bodyDef);

  PhysicsBoxDef boxDef;
  boxDef.SetRestitution(0);
  PhysicsTransform tx;
  PhysicsIdentity(tx);
  boxDef.Set(tx, glm::vec3(50.0f, 1.0f, 50.0f));
  body->AddBox(boxDef);

  bodyDef.bodyType = eDynamicBody;
  boxDef.Set(tx, glm::vec3(1.0f, 1.0f, 1.0f));

  for (int i = 0; i < 2; i++)
  {
    bodyDef.position = glm::vec3(0.0f + i * 0.5f, (2.0f * i) + 4, 0.0f);

    body = scene->CreateBody(bodyDef);
    body->AddBox(boxDef);
  }

  GameObjectBuilder builder;
  int couterbefore = 0;
  for (PhysicsBody *body = scene->BodyList(); body; body = body->Next())
  {
    manager.AddObject(builder.setBody(body).setMesh(meshes[0].get()).build());

    std::cout << "Body Position: \n"
              << "X: " << body->GetTransform().position.x << " Y: " << body->GetTransform().position.y << " Z: " << body->GetTransform().position.z << "\n";
  }

  Transform *clownTransform = new Transform();
  clownTransform->setPosition(glm::vec3(2.0, 2.0, 0.0));
  clownTransform->setRotation(glm::vec3(30.0, 0.0, 0.0));

  manager.AddObject(builder.setTransform(clownTransform).setMesh(meshes[1].get()).build());
}

void App::LoadMeshesAndStuff()
{
  meshes.push_back(std::make_unique<Mesh>(textureBoxVertexVector, "resources/textures/box.jpg", color_pick_shader.get()));
  meshes.push_back(std::make_unique<Mesh>(textureBoxVertexVector, "resources/textures/clown.png", color_pick_shader.get()));
}

void App::PhysicsUpdate(float time)
{

  // ! This is wrong, and not needed if time is working correctly
  // ! for now this is commented.
  // The time accumulator is used to allow the application to render at
  // a frequency different from the constant frequency the physics sim-
  // ulation is running at (default 60Hz).

  // static float accumulator = 0;
  //  accumulator += time;

  // accumulator = Clamp01(accumulator);
  // while (accumulator >= dt)
  //{
  scene->Step(time);
  //  accumulator -= dt;
  //}
}

void App::Run()
{
  bool bDone = false;
  double fNumOfTimes = 0;
  double fMediumTime = 0;

  LoadMeshesAndStuff();
  CreateGameObjects();

  mPhysicsTimerPtr->Reset();

  unsigned long lTempTime = GetApplicationTime();

  auto window = mAppWindow.GetWindow();
  PhysicsBody *bodyTest = scene->BodyList();

  int counter = 0;
  bool prawda = true;
  ImGuiIO &io = ImGui::GetIO();

  mFrameTime = 0;
  unsigned long lTempFrameTime = GetApplicationTime();

  while (!glfwWindowShouldClose(window))
  {

    // printf("Render \n");
    processInput(window);
    ProcessKey();
    ProcessMouse();

    // TODO make deltaTime work more robust
    // ? use http://gameprogrammingpatterns.com/game-loop.html ?
    // ? or even https://github.com/FrictionalGames/HPL1Engine ?
    // ? https://github.com/FrictionalGames/HPL1Engine/blob/master/sources/game/Game.cpp ?
    // * HPL implementation looks robust <3 for Penumbra

    // TODO change input timer to physicsTimer
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    while (mPhysicsTimerPtr->WantUpdate() && !pause)
    {
      unsigned int lUpdateTime = GetApplicationTime();

      PhysicsUpdate(mPhysicsTimerPtr->GetStepSize());

      unsigned int lDeltaTime = GetApplicationTime() - lUpdateTime;
      mUpdateTime = (float)(lDeltaTime) / 1000.0f;

      mIsUpdated = true;
      mGameTime += mPhysicsTimerPtr->GetStepSize();
    }
    mPhysicsTimerPtr->EndUpdateLoop();

    SetViewAndPerspective(mCamera);

    if (!io.WantCaptureMouse)
    {
      ColorPicking();
    }

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto shaderPtr = box_shader.get();

    for (auto gameobject : manager.objectList)
    {
      gameobject->Draw(*box_shader.get(), false, *this);
    }

    ImGuiStuff();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  g_clock.Stop();

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext(NULL);

  // printf("Close App.\n");
  glfwDestroyWindow(window);
  glfwTerminate();
}

unsigned long App::GetApplicationTime()
{
  return SDL_GetTicks();
}

// TODO fix
// broken behaviour
void App::ResetSimulation()
{

  for (int i = 0; i < 2; i++)
  {
    manager.objectList[i]->GetBody()->SetTransform(glm::vec3(0.0f + i * 0.5f, (2.0f * i) + 4, 0.0f));
    manager.objectList[i]->GetBody()->SetAngularVelocity(glm::vec3(0,0,0));
    manager.objectList[i]->GetBody()->SetLinearVelocity(glm::vec3(0,0,0));
    manager.objectList[i]->GetBody()->SetToAwake();
  }
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
    ImGui::SliderFloat("Left", &mCamera.orthographicSettings.left, -100.0f,
                       100.0f);
    ImGui::SliderFloat("Right", &mCamera.orthographicSettings.right, -100.0f,
                       100.0f);
    ImGui::SliderFloat("Bottom", &mCamera.orthographicSettings.bottom, -100.0f,
                       100.0f);
    ImGui::SliderFloat("Top", &mCamera.orthographicSettings.top, -100.0f,
                       100.0f);
    ImGui::SliderFloat("Near", &mCamera.orthographicSettings.zNear, -100.0f,
                       100.0f);
    ImGui::SliderFloat("Far", &mCamera.orthographicSettings.zFar, -100.0f,
                       200.0f);
  }

  if (ImGui::CollapsingHeader("Perspective"))
  {
    ImGui::SliderFloat("Fov", &mCamera.Zoom, 0.0f, 90.0f);
  }

  if (ImGui::CollapsingHeader("Collision", ImGuiTreeNodeFlags_DefaultOpen))
  {

    if (ImGui::Button("Pause/Resume"))
    {
      pause = !pause;
    }

    if (ImGui::Button("Reset"))
    {
      ResetSimulation();
    }

    if (ImGui::Button("PhysicsStep"))
    {
      scene->Step(1 / 60);
    }
  }

  if (ImGui::CollapsingHeader("Selected Object",
                              ImGuiTreeNodeFlags_DefaultOpen))
  {
    if (-1 != selectedObject)
    {
      GameObject *object = manager.GetSelectedObjectPtr(selectedObject);
      if (nullptr != object)
      {
        ImGui::Text("Position:");
        auto position = object->Position();
        ImGui::Text("X: %f Y: %f Z: %f", position.x, position.y, position.z);
      }
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

  for (auto gameobject : manager.objectList)
  {
    gameobject->Draw(*box_shader.get(), true, *this);
  }

  glFlush();
  glFinish();
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  if (Left_Mouse_click && mCanPick)
  {
    unsigned char data[4];

    // Assuming screen starts from left and window is somewhere up the ground
    glReadPixels(mousePosX, mAppWindow.Getheight() - mousePosY, 1, 1, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);

    int pickedID = data[0] + data[1] * 256 + data[2] * 256 * 256;

    selectedObject = pickedID;
    mCanPick = false;
  }
}

void App::SetViewAndPerspective(Camera &aCamera)
{
  if (aCamera.GetCameraMode() == Perspective)
  {
    aCamera.mProjection = glm::perspective(glm::radians(aCamera.Zoom),
                                           (float)mAppWindow.GetWidth() /
                                               (float)mAppWindow.Getheight(),
                                           0.1f, 10000.0f);
  }
  else
  {
    aCamera.mProjection =
        glm::ortho((double)mCamera.orthographicSettings.left,
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