#include "Application.h"
#include <iostream>
#include <stdio.h>
#include "Engine/miniaudio.h"

Application::Application(){
    if (ma_engine_init(NULL, &audioEngine) == MA_SUCCESS) {
        audioInitialized = true;
        // ma_engine_play_sound(&audioEngine, "assets/menu.ogg", NULL);
        std::cout << "Audio Engine Initialized\n";
    } else {
        std::cerr << "Failed to initialize audio engine\n";
    }

    playMusic("Resources/Music/start.wav");

    // if (ma_engine_play_sound(&audioEngine, "Resources/Music/test.wav", NULL) != MA_SUCCESS) {
    //     std::cerr << "Failed to play sound: Resources/Music/test.wav" << std::endl;
    //     std::cout << "Failed to play sound: Resources/Music/test.wav" << std::endl;
    // } else {
    //     std::cout << "Playing sound..." << std::endl;
    // }

    currentScreen = std::make_shared<MenuScreen>();
}

Application::~Application() {
    if (audioInitialized) {
        ma_engine_uninit(&audioEngine);
    }
}

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
        if (key == GLFW_KEY_ENTER && game_state == 0)
        {
            ma_engine_stop(&audioEngine); ma_engine_init(NULL, &audioEngine);
            playMusic("Resources/Music/mc.wav");
            std::cout << "Switching to Game Parkour Screen\n";
            setScreen(std::make_shared<BusScreen>());
            std::cout << "setscreen game parkour done" << std::endl;
        } 

        if (key == GLFW_KEY_ENTER && game_state == 1)
        {
            ma_engine_stop(&audioEngine); ma_engine_init(NULL, &audioEngine);
            playMusic("Resources/Music/tung.wav");
            std::cout << "Switching to GameScreen\n";
            setScreen(std::make_shared<GameScreen>());
        }

        if (key == GLFW_KEY_ENTER && game_state == -1)
        {
            std::cout << "Switching to LoseScreen\n";
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

    //     if (!audioInitialized) return;

    // ma_engine_stop(&audioEngine);
    // setMusicVolume(1.0f);
    // switch (game_state) {
    //     case 0: playMusic("Resources/Music/mc.wav"); break;
    //     case 1: playMusic("Resources/Music/tung.wav"); break;
    //     default:
    //         // No music for other states (0, -1)
    //         break;
    // }
    }
}

void Application::playMusic( std::string path) {
    setMusicVolume(1.0f);
    if (!audioInitialized) {
        std::cerr << "Audio Engine Not Initialized\n";
        return;
    }
    // ma_engine_stop(&audioEngine);  // Stop any current music
    ma_result result = ma_engine_play_sound(&audioEngine, path.c_str(), NULL);
    if (result != MA_SUCCESS) {
        std::cerr << "Failed to play music: " << path << " | Error: " << result << "\n";
    } else {
        std::cout << "Playing: " << path << "\n";
    }
}

void Application::stopMusic() {
    if (audioInitialized) {
        ma_engine_stop(&audioEngine);
    }
}

void Application::setMusicVolume(float volume) {
    if (audioInitialized) {
        ma_engine_set_volume(&audioEngine, volume);
    }
}
