#pragma once

#include <GL/glew.h>

#include <GLFW/glfw3.h>

namespace AppWindow
{
    class AppWindow
    {

        GLFWwindow *mWindow;
        unsigned int mWidth;
        unsigned int mHeight;
        int init(int width, int height);

    public:
        AppWindow(unsigned int width, unsigned int height);
        GLFWwindow *GetWindow();
        unsigned int GetWidth() { return mWidth; }
        unsigned int Getheight() { return mHeight; }
    };

}