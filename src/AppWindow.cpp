#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "AppWindow.h"

#include "stb_image.h"
#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

namespace AppWindow
{

    AppWindow::AppWindow(unsigned int width, unsigned int height) : mWidth(width),
                                                                    mHeight(height)
    {
        init(width, height);
    }

    GLFWwindow *AppWindow::GetWindow()
    {
        return mWindow;
    }

    int AppWindow::init(int width, int height)
    {

        const char *glsl_version = "#version 450";

        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        mWindow = glfwCreateWindow(width, height, "Playground", NULL, NULL);
        if (mWindow == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(mWindow);
        glfwSwapInterval(0);

#pragma region IMGUI
        // IMGUI
        // setup Imgui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;

        stbi_set_flip_vertically_on_load(true);

        ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
        ImGui_ImplOpenGL3_Init(glsl_version);

// -------------------------------------------------------
#pragma endregion IMGUI

        // Initialize GLEW
        bool gl3w_err = glewInit() != 0;
        if (gl3w_err)
        {
            fprintf(stderr, "Failed to initialize GLEW\n");
            glfwTerminate();
            abort();
        }

        glViewport(0, 0, width, height);

        glfwSetFramebufferSizeCallback(mWindow, framebuffer_size_callback);
        // glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        glEnable(GL_DEPTH_TEST);

        return 0;
    }

}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}