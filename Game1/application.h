#pragma once

#include "Game1/menuscreen.h"
#include "Game1/gamescreen.h"
#include "Graphics/global.h"
#include <memory>

#include <GLFW/glfw3.h>

class Application
{
public:
    Application();
    ~Application();
    void update(double deltaTime);
    void draw();
    void keyEvent(int key, int action);
    void mousePosEvent(double xpos, double ypos);
    void mouseButtonEvent(int button, int action);
    void scrollEvent(double distance);
    void windowResizeEvent(int width, int height);
    void framebufferResizeEvent(int width, int height);
    void setScreen(std::shared_ptr<Screen> newScreen);

private:
    std::shared_ptr<Screen> currentScreen;
};
