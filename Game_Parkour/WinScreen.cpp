#include "WinScreen.h"
#include <iostream>
#include "Game.h"
#include "Game1/application.h"


WinScreen::WinScreen(std::shared_ptr<ScreenManager> manager) : screenManager(manager) {
    InitializeMenu("Win");
    std::cout << "Starting WinScreen" << std::endl;
}

void WinScreen::InitializeMenu(std::string game_result) {
    // std::cout << "Starting WinScreen init" << std::endl;
    std::cout << "Starting Menu" << std::endl;

    gameWorld = std::make_shared<GameWorld>();
    // std::cout << "Starting WinScreen player" << std::endl;

    auto uiText = std::make_shared<GameObject_Yang>(9999);
    uiText->AddComponent<UIComponent>("Game Over! You " + game_result);
    gameWorld->AddGameObject(uiText);

    auto cameraSystem = std::make_shared<CameraSystem>();
    gameWorld->AddSystem(cameraSystem);

    auto drawSystem = std::make_shared<DrawSystem_Yang>("text");
    gameWorld->AddSystem(drawSystem);

    std::cout << "ENd init Menu" << std::endl;



}

void WinScreen::update(double deltaTime) {
    if(screenManager->GetCurrentScreen().get() != this) return;

    gameWorld->Update(deltaTime);
    auto& inputMap = gameWorld->GetInputMap();
    Application::game_state = 1;
    if (inputMap.IsKeyPressed(GLFW_KEY_ENTER)) {
        Game::switchToScreen("MENU");

        gameWorld->GetInputMap().SetKeyState(GLFW_KEY_ENTER, false);

    }
}

void WinScreen::draw() {
    gameWorld->Draw();
    
}

void WinScreen::keyEvent(int key, int action) {
    if(screenManager->GetCurrentScreen().get() != this) return;

    // bool isPressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
    bool isPressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
    gameWorld->GetInputMap().SetKeyState(key, isPressed);
}

void WinScreen::mousePosEvent(double xpos, double ypos) {

}

void WinScreen::mouseButtonEvent(int button, int action) {

}

void WinScreen::scrollEvent(double distance) {

}

void WinScreen::framebufferResizeEvent(int width, int height) {
    Global::graphics.setFramebufferSize(glm::ivec2(width, height));
}


void WinScreen::windowResizeEvent(int width, int height) {
    auto cameraSystem = gameWorld->GetSystem<CameraSystem>();

    if (cameraSystem) {
        cameraSystem->windowResizeEvent(width, height);
    }
}
