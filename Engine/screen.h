#pragma once

#include "Graphics/global.h"
#include <GLFW/glfw3.h>
#include "gameobject.h"
#include "system.h"

class Screen
{
public:
    Screen() = default;
    virtual ~Screen() = default;
    virtual void update(double deltaTime) = 0;
    virtual void draw() = 0;
    virtual void keyEvent(int key, int action) = 0;
    virtual void mousePosEvent(double xpos, double ypos) = 0;
    virtual void mouseButtonEvent(int button, int action) = 0;
    virtual void scrollEvent(double distance) = 0;
    virtual void windowResizeEvent(int width, int height) = 0;
    virtual void framebufferResizeEvent(int width, int height) = 0;


private:
    std::shared_ptr<Camera> cam;
    std::shared_ptr<Shape> shp;
    std::shared_ptr<ModelTransform> transform;
};
