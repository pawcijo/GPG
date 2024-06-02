#include <AppWindow.h>
#include <App.h>

//#define DYNAMIC_FRICTION true

int main()
{

    AppWindow::AppWindow appWindow(1920, 1080);
    App app(appWindow);

    app.Run();

    return 0;
}
