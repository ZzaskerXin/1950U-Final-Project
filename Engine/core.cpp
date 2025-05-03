#include "core.h"

Core::Core() /*:*/
    // cam(std::make_shared<Camera>()),
    // transform(std::make_shared<ModelTransform>())
{
    // shp = Global::graphics.getShape("sphere");

    // transform->translate(glm::vec3(0.0f, 0.0f, 2.0f));
    // transform->rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    // transform->scale(glm::vec3(1.0f, 1.0f, 1.0f));

    // Global::graphics.addShape("test", "Resourc   es/Meshes/ceiling.obj");
    application = std::make_shared<Application>();
}

Core::~Core(){
}

void Core::update(double deltaTime){
    // glm::vec3 look = cam->getLook();

    // glm::vec3 movementDirection(0.0f);

    // if (wKeyPressed) {
    //     movementDirection += glm::vec3(look.x, 0.0f, look.z);
    // }
    // if (sKeyPressed) {
    //     movementDirection += glm::vec3(-look.x, 0.0f, -look.z);
    // }
    // if (aKeyPressed) {
    //     movementDirection += glm::vec3(look.z, 0.0f, -look.x);
    // }
    // if (dKeyPressed) {
    //     movementDirection += glm::vec3(-look.z, 0.0f, look.x);
    // }

    // if (glm::length(movementDirection) > 0.0f) {
    //     movementDirection = glm::normalize(movementDirection);
    // }

    // float speed = 5.0f;

    // cam->translate(movementDirection * speed * static_cast<float>(deltaTime));
    if (application)
    {
        application->update(deltaTime);
    }
}

void Core::draw(){
    // Global::graphics.setClearColor(glm::vec4(0.2f, 0.6f, 1.0f, 1.0f));
    // Global::graphics.clearScreen(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Global::graphics.bindShader("phong");
    // Global::graphics.setGlobalData(glm::vec3(0.5f));

    // Global::graphics.setCameraData(cam);

    // Global::graphics.drawShape(shp, transform, Global::graphics.addMaterial("texture",  "Resources/Images/grass.png", 1));

    // Global::graphics.bindShader("text");
    // Global::graphics.drawUIText(Global::graphics.getFont("opensans"), "Welcome to CS1950U!", glm::ivec2(80, 260), AnchorPoint::TopLeft, Global::graphics.getFramebufferSize().x, 1.f, 0.1f, glm::vec3(1, 0, 1));
    if (application)
    {
        application->draw();
    }
}

void Core::keyEvent(int key, int action){
    // if (action == GLFW_PRESS) {
    //     if (key == GLFW_KEY_W) {
    //         wKeyPressed = true;
    //     } else if (key == GLFW_KEY_A) {
    //         aKeyPressed = true;
    //     } else if (key == GLFW_KEY_S) {
    //         sKeyPressed = true;
    //     } else if (key == GLFW_KEY_D) {
    //         dKeyPressed = true;
    //     }
    // } else if (action == GLFW_RELEASE) {
    //     if (key == GLFW_KEY_W) {
    //         wKeyPressed = false;
    //     } else if (key == GLFW_KEY_A) {
    //         aKeyPressed = false;
    //     } else if (key == GLFW_KEY_S) {
    //         sKeyPressed = false;
    //     } else if (key == GLFW_KEY_D) {
    //         dKeyPressed = false;
    //     }
    // }
    if (application)
    {
        application->keyEvent(key, action);
    }
}

void Core::mousePosEvent(double xpos, double ypos){
    // if (rightMousePressed) {
    //     glm::vec2 currentMousePos(xpos, ypos);
    //     glm::vec2 deltaMousePos = currentMousePos - prevMousePos;


    //     float sensitivity = 0.001f;


    //     cam->rotate(deltaMousePos.x * sensitivity, glm::vec3(0.0f, 1.0f, 0.0f));


    //     glm::vec3 look = cam->getLook();


    //     cam->rotate(deltaMousePos.y * sensitivity, glm::vec3(look.z, 0.0f, -look.x));


    //     prevMousePos = currentMousePos;
    // } else {
    //     prevMousePos = glm::vec2(xpos, ypos);
    // }
    if (application)
    {
        application->mousePosEvent(xpos, ypos);
    }
}

void Core::mouseButtonEvent(int button, int action){
    // if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    //     if (action == GLFW_PRESS) {
    //         rightMousePressed = true;
    //     } else if (action == GLFW_RELEASE) {
    //         rightMousePressed = false;
    //     }
    // }
    if (application)
    {
        application->mouseButtonEvent(button, action);
    }
}

void Core::scrollEvent(double distance){
    if (application)
    {
        application->scrollEvent(distance);
    }
}

void Core::framebufferResizeEvent(int width, int height){
    application->framebufferResizeEvent(width, height);
}

void Core::windowResizeEvent(int width, int height){
    application->windowResizeEvent(width, height);
}
