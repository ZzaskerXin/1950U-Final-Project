#include "Game.h"
#include "ScreenManager.h"
#include "GameScreen.h"
#include "MenuScreen.h"
#include "WinScreen.h"
#include "LossScreen.h"
#include "Game1/application.h"
#include <iostream>

// Global ScreenManager
std::shared_ptr<ScreenManager> screenManager;

void Game::Initialize(std::shared_ptr<ScreenManager> manager) {
    if (!manager) {
        std::cerr << "ERROR: ScreenManager is NULL in Game::Initialize!" << std::endl;
        return;
    }

    screenManager = manager; // 🔥 FIX: Assign the passed manager to the global screenManager

    std::cout << "Initializing game with ScreenManager: " << screenManager.get() << std::endl;

    // Start the game on the Menu Screen
    if(Application::game_state == 0) {
        screenManager->switchToScreen("GAME", std::make_shared<GameScreen_Parkour>(screenManager));
    }
    else if(Application::game_state == -1) {
        screenManager->switchToScreen("LOSS", std::make_shared<GameScreen_Parkour>(screenManager));

    }

    else if(Application::game_state == 3) {
        screenManager->switchToScreen("WIN2", std::make_shared<GameScreen_Parkour>(screenManager));

    }

    std:: cout << "SCreenManager init finished!" << std:: endl;
} 

void Game::switchToScreen(const std::string& screenName) {
    if (!screenManager) {
        std::cerr << "ERROR: screenManager is NULL in Game::switchToScreen!" << std::endl;
        return;
    }

    std::cout << "Attempting to switch to screen: " << screenName << std::endl;

    if (screenName == "GAME") {
        screenManager->switchToScreen("GAME", std::make_shared<GameScreen_Parkour>(screenManager));
    } else if (screenName == "MENU") {
        screenManager->switchToScreen("MENU", std::make_shared<MenuScreen_Parkour>(screenManager));
    } else if (screenName == "WIN") {
        screenManager->switchToScreen("WIN", std::make_shared<WinScreen>(screenManager));
    } else if (screenName == "LOSS") {
        screenManager->switchToScreen("LOSS", std::make_shared<LossScreen>(screenManager));
    } else {
        std::cerr << "Unknown screen: " << screenName << std::endl;
    }
}
