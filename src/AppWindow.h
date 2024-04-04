#pragma once

#if defined (__APPLE_CC__)
#include <OpenGL/gl3.h>
#else
#include <GL/gl3.h>       /* assert OpenGL 3.2 core profile available. */
#endif

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