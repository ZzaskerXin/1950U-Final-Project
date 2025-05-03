#pragma once
#include <memory>
#include <unordered_map>
#include <string>

class Screen;

class ScreenManager {
public:
    ScreenManager();

    void switchToScreen(std::string name, std::shared_ptr<Screen> newScreen);
    void update(double deltaTime);
    void draw();
    void keyEvent(int key, int action);
    void mousePosEvent(double xpos, double ypos);
    void mouseButtonEvent(int button, int action);
    void windowResizeEvent(int width, int height);
    void framebufferResizeEvent(int width, int height);
    void scrollEvent(double distance);
    
    std::shared_ptr<Screen> GetCurrentScreen() const { return currentScreen; } 

private:
    std::shared_ptr<Screen> currentScreen;
};
