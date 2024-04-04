#ifdef __APPLE__
/* Defined before OpenGL and GLUT includes to avoid deprecation messages */
#define GL_SILENCE_DEPRECATION
#endif

#include <AppWindow.h>
#include <App.h>



int main()
{

    AppWindow::AppWindow appWindow(1600, 900);
    App app(appWindow);

    app.Run();

    return 0;
}
