#pragma once

#include <AppWindow.h>
#include <SimpleShape/Square.h>

#include <vector>

class App
{

    bool F1_Pressed = false;

    AppWindow::AppWindow &mAppWindow;
    std::vector<Square*> mSquares;
    DrawMode mDrawMode = DrawMode::EDefault;

    void SwitchDrawMode();

    void ProcessKey();

public:
    App(AppWindow::AppWindow &window);
    void Run();
};