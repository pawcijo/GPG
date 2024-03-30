#pragma once

#include <AppWindow.h>

class App
{
    AppWindow::AppWindow& mAppWindow;
public:
    App(AppWindow::AppWindow& window);
    void Run();
};