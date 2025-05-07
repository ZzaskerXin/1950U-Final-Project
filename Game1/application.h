#pragma once

#include "Game1/menuscreen.h"
#include "Game1/gamescreen.h"
#include "Game1/bus_screen.h"
#include "Graphics/global.h"
#include <memory>
#include "Engine/miniaudio.h"
#include <GLFW/glfw3.h>

class Application
{
public:
    Application();
    ~Application();
    void update(double deltaTime);
    void draw();
    void keyEvent(int key, int action);
    void mousePosEvent(double xpos, double ypos);
    void mouseButtonEvent(int button, int action);
    void scrollEvent(double distance);
    void windowResizeEvent(int width, int height);
    void framebufferResizeEvent(int width, int height);
    void setScreen(std::shared_ptr<Screen> newScreen);

    inline static int game_state = 0; 

    void playMusic( std::string path);
void stopMusic();
void setMusicVolume(float volume);


private:
    std::shared_ptr<Screen> currentScreen;
    // 0 for menu, 1 if game parkour is done, 2 if game survive is done

    ma_engine audioEngine;
    bool audioInitialized = false;
};
