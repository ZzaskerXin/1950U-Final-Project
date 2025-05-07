#include "LossScreen.h"
#include <iostream>


LossScreen::LossScreen(std::shared_ptr<ScreenManager> manager) : screenManager(manager) {
    InitializeMenu("Loss");
    std::cout << "Starting LossScreen" << std::endl;
}

void LossScreen::InitializeMenu(std::string game_result) {
    // std::cout << "Starting LossScreen init" << std::endl;
    std::cout << "Starting Menu" << std::endl;

    gameWorld = std::make_shared<GameWorld>();
    // std::cout << "Starting LossScreen player" << std::endl;

    auto uiText = std::make_shared<GameObject_Yang>(9999);
    uiText->AddComponent<UIComponent>("Game Over! You failed to beat his dark plot!",  "DejaVuSans", glm::ivec2(130, 350));
    // Global::graphics.drawUIText(Global::graphics.getFont("DejaVuSans"), "Game Over! You failed to beat his dark plot!", glm::ivec2(130, 350), AnchorPoint::TopLeft, Global::graphics.getFramebufferSize().x, 0.5f, 0.1f, glm::vec3(1, 0, 0));

    gameWorld->AddGameObject(uiText);

    auto cameraSystem = std::make_shared<CameraSystem>();
    gameWorld->AddSystem(cameraSystem);

    auto drawSystem = std::make_shared<DrawSystem_Yang>("text");
    gameWorld->AddSystem(drawSystem);

    std::cout << "ENd init Menu" << std::endl;



}

void LossScreen::update(double deltaTime) {
    if(screenManager->GetCurrentScreen().get() != this) return;
    gameWorld->Update(deltaTime);
    auto& inputMap = gameWorld->GetInputMap();
    if (inputMap.IsKeyPressed(GLFW_KEY_ENTER)) {
        Game::switchToScreen("MENU");
        
        gameWorld->GetInputMap().SetKeyState(GLFW_KEY_ENTER, false);

    }
}

void LossScreen::draw() {
    if(screenManager->GetCurrentScreen().get() != this) return;

    gameWorld->Draw();
    
}

void LossScreen::keyEvent(int key, int action) {
    if(screenManager->GetCurrentScreen().get() != this) return;

    bool isPressed = (action == GLFW_RELEASE);
    gameWorld->GetInputMap().SetKeyState(key, isPressed);
}

void LossScreen::mousePosEvent(double xpos, double ypos) {

}

void LossScreen::mouseButtonEvent(int button, int action) {

}

void LossScreen::scrollEvent(double distance) {

}

void LossScreen::framebufferResizeEvent(int width, int height) {
    Global::graphics.setFramebufferSize(glm::ivec2(width, height));
}


void LossScreen::windowResizeEvent(int width, int height) {
    auto cameraSystem = gameWorld->GetSystem<CameraSystem>();

    if (cameraSystem) {
        cameraSystem->windowResizeEvent(width, height);
    }
}
