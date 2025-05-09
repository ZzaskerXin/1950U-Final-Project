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

    std::vector<std::string> lines = {
        "You succeed! Now you have teleported to Tung Tung Tung Sahur's base.",
        "Now there are only small Tung Tungs, but they can also threaten you.",
        "Survive to show Tung Tung Tung Sahur your power to mess up his home,",
        "so that he will give up his plot."
    };
    
    int baseId = 9991;
    int baseY = 350;
    int lineSpacing = 40;
    
    for (int i = 0; i < lines.size(); ++i) {
        auto uiText = std::make_shared<GameObject_Yang>(baseId + i);
        uiText->AddComponent<UIComponent>(
            lines[i],
            "DejaVuSans",
            glm::ivec2(130, baseY + i * lineSpacing)
        );
        gameWorld->AddGameObject(uiText);
    }
    
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
