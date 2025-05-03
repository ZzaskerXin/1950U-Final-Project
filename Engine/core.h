#pragma once

#include "Graphics/global.h"
#include "Game1/application.h"

#include <GLFW/glfw3.h>

class Core
{
public:
    Core();
    ~Core();
    void update(double deltaTime);
    void draw();
    void keyEvent(int key, int action);
    void mousePosEvent(double xpos, double ypos);
    void mouseButtonEvent(int button, int action);
    void scrollEvent(double distance);
    void windowResizeEvent(int width, int height);
    void framebufferResizeEvent(int width, int height);
private:
    std::shared_ptr<Application> application;
    std::shared_ptr<Camera> cam;
    std::shared_ptr<Shape> shp;
    std::shared_ptr<ModelTransform>transform;

    bool wKeyPressed = false;
    bool aKeyPressed = false;
    bool sKeyPressed = false;
    bool dKeyPressed = false;

    bool rightMousePressed = false;
    glm::vec2 prevMousePos;
};
