#pragma once

#include "Engine/screen.h"
#include "Screen/ScreenManager.h"

class BusScreen : public Screen
{
public:
    BusScreen();
    ~BusScreen();

    void update(double deltaTime);
    void draw();
    void keyEvent(int key, int action);
    void mousePosEvent(double xpos, double ypos);
    void mouseButtonEvent(int button, int action);
    void scrollEvent(double distance);
    void windowResizeEvent(int width, int height);
    void framebufferResizeEvent(int width, int height);
private:
    std::shared_ptr<ScreenManager> screenManager;
};
