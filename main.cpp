#include <AppWindow.h>
#include <App.h>

//#define DYNAMIC_FRICTION true

int main()
{

    AppWindow::AppWindow appWindow(1600, 900);
    App app(appWindow);

    app.Run();

    return 0;
}
