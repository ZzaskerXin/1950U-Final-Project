#include "MenuScreen.h"
#include <iostream>


MenuScreen_Parkour::MenuScreen_Parkour(std::shared_ptr<ScreenManager> manager) : screenManager(manager) {
    InitializeMenu();
    std::cout << "Starting MenuScreen_Parkour" << std::endl;
}

void MenuScreen_Parkour::InitializeMenu() {
    // std::cout << "Starting MenuScreen_Parkour init" << std::endl;
    std::cout << "Starting Menu for parkour game" << std::endl;

    gameWorld = std::make_shared<GameWorld>();
    std::cout << "Starting MenuScreen_Parkour player" << std::endl;

    auto uiText = std::make_shared<GameObject_Yang>(9999);
    uiText->AddComponent<UIComponent>("Welcome to Daoli Parkour");
    gameWorld->AddGameObject(uiText);
    std::cout << "add ui" << std::endl;


    auto cameraSystem = std::make_shared<CameraSystem>();
    std::cout << "cam sys created" << std::endl;

    gameWorld->AddSystem(cameraSystem);
    std::cout << "add cam" << std::endl;


    auto drawSystem = std::make_shared<DrawSystem_Yang>("text");
    gameWorld->AddSystem(drawSystem);
    std::cout << "add draw sys" << std::endl;


    std::cout << "ENd init Menu" << std::endl;



}

void MenuScreen_Parkour::update(double deltaTime) {
    if(screenManager->GetCurrentScreen().get() != this) return;

    gameWorld->Update(deltaTime);
    auto& inputMap = gameWorld->GetInputMap();
    if (inputMap.IsKeyPressed(GLFW_KEY_ENTER)) {
        gameWorld->GetInputMap().SetKeyState(GLFW_KEY_ENTER, false);
        // screenManager->switchToScreen("GAME");
        Game::switchToScreen("GAME");
    }
}

void MenuScreen_Parkour::draw() {
    if(screenManager->GetCurrentScreen().get() != this) return;
    std::cout << "menu parkour drawing... " << std::endl;
    gameWorld->Draw();
    
}

void MenuScreen_Parkour::keyEvent(int key, int action) {
    if(screenManager->GetCurrentScreen().get() != this) return;
    
    bool isPressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
    gameWorld->GetInputMap().SetKeyState(key, isPressed);
}

void MenuScreen_Parkour::mousePosEvent(double xpos, double ypos) {

}

void MenuScreen_Parkour::mouseButtonEvent(int button, int action) {

}

void MenuScreen_Parkour::scrollEvent(double distance) {

}

void MenuScreen_Parkour::framebufferResizeEvent(int width, int height) {
    Global::graphics.setFramebufferSize(glm::ivec2(width, height));
}


void MenuScreen_Parkour::windowResizeEvent(int width, int height) {
    auto cameraSystem = gameWorld->GetSystem<CameraSystem>();

    if (cameraSystem) {
        cameraSystem->windowResizeEvent(width, height);
    }
}
