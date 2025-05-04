#include "ScreenManager.h"
#include "Screen.h"
#include <iostream>
#include <GLFW/glfw3.h>

ScreenManager::ScreenManager() : currentScreen(nullptr) {}

#include <cassert>

void ScreenManager::switchToScreen( std::string name, std::shared_ptr<Screen_Yang> newScreen) {
    // std::cout << "ScreenManager address: " << this << std::endl;

    // std::cerr << "Attempting to switch to: " << name << std::endl;

    // assert(this != nullptr && "ScreenManager is NULL!");  // Check if ScreenManager itself is corrupt
    // assert(newScreen != nullptr && "New screen is NULL!");  // If newScreen is NULL, it is being deleted before passing here

    if (currentScreen) {
        // std::cerr << "Resetting current screen: " << currentScreen.get() << std::endl;
        currentScreen.reset();  
        // std::cerr << "Reset complete" << std::endl;
    } else {
        // std::cerr << "No current screen to reset." << std::endl;
    }

    // std::cerr << "Moving new screen..." << std::endl;
    currentScreen = std::move(newScreen);  
    int width, height;
    glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);
    if (currentScreen) {
        std::cout << "new screen with name" << name << std::endl; 
        currentScreen->windowResizeEvent(width, height);
        currentScreen->framebufferResizeEvent(width, height);
    }
    std::cout << "Switched to screen: " << name << std::endl;
}

void ScreenManager::update(double deltaTime) {
    if (currentScreen) currentScreen->update(deltaTime);
}

void ScreenManager::draw() {
    if (currentScreen) currentScreen->draw();
}

void ScreenManager::keyEvent(int key, int action) {
    if (currentScreen) currentScreen->keyEvent(key, action);
}

void ScreenManager::mousePosEvent(double xpos, double ypos) {
    if (currentScreen) currentScreen->mousePosEvent(xpos, ypos);
}

void ScreenManager::scrollEvent(double distance){
    if (currentScreen) currentScreen->scrollEvent(distance);
}

void ScreenManager::mouseButtonEvent(int button, int action) {
    if (currentScreen) currentScreen->mouseButtonEvent(button, action);
}

void ScreenManager::windowResizeEvent(int width, int height) {
    if (currentScreen) currentScreen->windowResizeEvent(width, height);
}

void ScreenManager::framebufferResizeEvent(int width, int height) {
    if (currentScreen) currentScreen->framebufferResizeEvent(width, height);
}
