#include <AppWindow.h>
#include <App.h>

int main()
{

      AppWindow::AppWindow appWindow(1600,900);
      App app(appWindow);

      app.Run();

    return 0;
}

