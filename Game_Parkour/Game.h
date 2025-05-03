#pragma once
#include <memory>
#include <string>

class ScreenManager;

class Game {
public:
    static void Initialize(std::shared_ptr<ScreenManager> screenManager);
    static void switchToScreen(const std::string& screenName);
};
