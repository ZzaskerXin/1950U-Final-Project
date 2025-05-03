#include "Application.h"
#include <iostream>

Application::Application(){
    currentScreen = std::make_shared<MenuScreen>();
}

Application::~Application(){}

void Application::update(double deltaTime)
{
    if (currentScreen)
        currentScreen->update(deltaTime);
}


void Application::draw()
{
    if (currentScreen)
        currentScreen->draw();
}

void Application::keyEvent(int key, int action)
{
    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_ENTER)
        {
            std::cout << "Switching to GameScreen\n";
            setScreen(std::make_shared<GameScreen>());
        }

        if (key == GLFW_KEY_BACKSPACE)
        {
            std::cout << "Switching to MenuScreen\n";
            setScreen(std::make_shared<MenuScreen>());
        }
    }
    if (currentScreen)
        currentScreen->keyEvent(key, action);
}

void Application::mousePosEvent(double xpos, double ypos)
{
    if (currentScreen)
        currentScreen->mousePosEvent(xpos, ypos);
}

void Application::mouseButtonEvent(int button, int action)
{
    if (currentScreen)
        currentScreen->mouseButtonEvent(button, action);
}

void Application::scrollEvent(double distance)
{
    if (currentScreen)
        currentScreen->scrollEvent(distance);
}

void Application::windowResizeEvent(int width, int height)
{
    if (currentScreen)
        currentScreen->windowResizeEvent(width, height);
}

void Application::framebufferResizeEvent(int width, int height)
{
    if (currentScreen)
        currentScreen->framebufferResizeEvent(width, height);
}

void Application::setScreen(std::shared_ptr<Screen> newScreen)
{
    if (newScreen)
    {
        currentScreen = newScreen;
    }
}
