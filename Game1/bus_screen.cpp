#include "bus_screen.h"
#include "Game_Parkour/Game.h"


BusScreen ::BusScreen()
{
    screenManager = std::make_shared<ScreenManager>();
    Game::Initialize(screenManager);
    std:: cout << "BusScreen inited success " << screenManager << std::endl;
}

BusScreen ::~BusScreen(){}

void BusScreen::update(double deltaTime){
    screenManager->update(deltaTime);
}

void BusScreen::draw(){
    screenManager->draw();
}

void BusScreen::keyEvent(int key, int action){
    screenManager->keyEvent(key,action);
}

void BusScreen::mousePosEvent(double xpos, double ypos){
    screenManager->mousePosEvent(xpos, ypos);
}

void BusScreen::mouseButtonEvent(int button, int action){
    screenManager->mouseButtonEvent(button, action);
}

void BusScreen::scrollEvent(double distance){
    screenManager->scrollEvent(distance);
}

void BusScreen::framebufferResizeEvent(int width, int height){
    screenManager->framebufferResizeEvent(width,height);
}

void BusScreen::windowResizeEvent(int width, int height){
    screenManager->windowResizeEvent(width,height);
}
