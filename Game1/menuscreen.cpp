#include "menuscreen.h"

MenuScreen ::MenuScreen():
    cam(std::make_shared<Camera>())
{
    titleText = "Dodge the Pillars";
}

MenuScreen ::~MenuScreen(){}

void MenuScreen::update(double deltaTime){
    // if (w_pressed)
    // {
    //     titleText = "New Text";
    // }
    // else if (s_pressed)
    // {
    //     titleText = "My First Game";
    // }
}

void MenuScreen::draw(){
    Global::graphics.setClearColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    Global::graphics.clearScreen(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Global::graphics.bindShader("text");
    Global::graphics.drawUIText(Global::graphics.getFont("opensans"), titleText, glm::ivec2(80, 260), AnchorPoint::TopLeft, Global::graphics.getFramebufferSize().x, 1.f, 0.1f, glm::vec3(0, 0, 0));
    Global::graphics.drawUIText(Global::graphics.getFont("opensans"), "Press Enter to Start", glm::ivec2(82, 200), AnchorPoint::TopLeft, Global::graphics.getFramebufferSize().x, 0.5f, 0.1f, glm::vec3(0, 0, 0));
}

void MenuScreen::keyEvent(int key, int action){
    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_W) {
            w_pressed = true;
        }
        if (key == GLFW_KEY_S) {
            s_pressed = true;
        }
    }
    else if (action == GLFW_RELEASE)
    {
        if (key == GLFW_KEY_W)
        {
            w_pressed = false;
        }
        else if (key == GLFW_KEY_S)
        {
            s_pressed = false;
        }
    }
}

void MenuScreen::mousePosEvent(double xpos, double ypos){

}

void MenuScreen::mouseButtonEvent(int button, int action){
}

void MenuScreen::scrollEvent(double distance){
}

void MenuScreen::framebufferResizeEvent(int width, int height){
    Global::graphics.setFramebufferSize(glm::ivec2(width, height));
}

void MenuScreen::windowResizeEvent(int width, int height){
    Global::graphics.setWindowSize(glm::ivec2(width, height));
    cam->resize(width, height);
}
