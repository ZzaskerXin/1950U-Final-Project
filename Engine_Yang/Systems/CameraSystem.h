#pragma once
#include "System.h"
#include "Graphics/global.h"
#include "GameWorld.h"
#include "TransformComponent.h"
#include <memory>

class CameraSystem : public System {
private:
    std::shared_ptr<Camera> camera;
    float yaw = 0.0f, pitch = 0.0f;
    const float pitchLimit = 89.0f;
    bool rightMousePressed = false;
    glm::vec2 prevMousePos = glm::vec2(0.0f, 0.0f);
    float thirdPersonDistance = 3.0f;

    enum class CameraMode { FirstPerson, ThirdPerson };
    CameraMode cameraMode = CameraMode::FirstPerson;
    bool keyVPressed = false;

public:
    CameraSystem() : camera(std::make_shared<Camera>()) {}

    void Update(float deltaTime) override {
        if (!gameWorld) return;
        auto& inputMap = gameWorld->GetInputMap();

        if (inputMap.IsKeyPressed(GLFW_KEY_V) && !keyVPressed) {
            keyVPressed = true; // prevent multiple toggles while the key is held
            if (cameraMode == CameraMode::FirstPerson) {
                cameraMode = CameraMode::ThirdPerson;
            } else {
                cameraMode = CameraMode::FirstPerson;
            }
        } else if (!inputMap.IsKeyPressed(GLFW_KEY_V)) {
            keyVPressed = false; // reset the state when the key is released
        }

        for (auto& obj : objects) {
            auto* transform = obj->GetComponent<TransformComponent>();
            if (transform) {
                // Set camera position based on the camera mode
                if (cameraMode == CameraMode::FirstPerson) {
                    camera->setPos(transform->transform.getPos());
                } else { // Third person
                    glm::vec3 thirdPersonOffset = -camera->getLook() * thirdPersonDistance + glm::vec3(0.0f, 1.0f, 0.0f);
                    camera->setPos(transform->transform.getPos() + thirdPersonOffset);
                }
            }
        }
    }

    void mousePosEvent(double xpos, double ypos) {
        glm::vec2 currentMousePos(xpos, ypos);

        if (!rightMousePressed) {
            prevMousePos = currentMousePos;
            return;
        }

        glm::vec2 delta = currentMousePos - prevMousePos;
        prevMousePos = currentMousePos;

        float sensitivity = 0.3f;
        yaw += delta.x * sensitivity;
        pitch -= delta.y * sensitivity;

        pitch = glm::clamp(pitch, -pitchLimit, pitchLimit);

        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        camera->setLook(glm::normalize(direction));
    }

    void mouseButtonEvent(int button, int action) {
        if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            if (action == GLFW_PRESS)
                rightMousePressed = true;
            else if (action == GLFW_RELEASE)
                rightMousePressed = false;
        }
    }

    void scrollEvent(double distance) {
        thirdPersonDistance -= distance;
        thirdPersonDistance = glm::clamp(thirdPersonDistance, 1.0f, 10.0f);

        if (fabs(distance) > 0.01f) {
            float currentFOV = camera->getFOV();
            currentFOV -= distance * 2.0f;
            currentFOV = glm::clamp(currentFOV, 30.0f, 90.0f);
            camera->setFOV(currentFOV);
        }
    }

    void framebufferResizeEvent(int width, int height) {
        Global::graphics.setFramebufferSize(glm::ivec2(width, height));
    }

    void windowResizeEvent(int width, int height) {
        Global::graphics.setWindowSize(glm::ivec2(width, height));
        camera->resize(width, height);
    }

    

    std::shared_ptr<Camera> GetCamera() { return camera; }
};
