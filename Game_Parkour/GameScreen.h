#pragma once
#include "Screen.h"
#include "ScreenManager.h"
#include "GameWorld.h"
#include "CharacterControllerSystem.h"
#include "DrawSystem.h"
#include "CameraSystem.h"
#include "CollisionSystem.h"
#include "Game.h"

class GameScreen_Parkour : public Screen {
public:
    GameScreen_Parkour(std::shared_ptr<ScreenManager> manager);
    void update(double deltaTime) override;
    void draw() override;
    void keyEvent(int key, int action) override;

    void mousePosEvent(double xpos, double ypos) override;
    void mouseButtonEvent(int button, int action) override;
    void scrollEvent(double distance) override;
    void windowResizeEvent(int width, int height) override;
    void framebufferResizeEvent(int width, int height) override;


private:
    std::shared_ptr<ScreenManager> screenManager;
    std::shared_ptr<GameWorld> gameWorld;

    bool rightMousePressed;
    glm::vec2 prevMousePos;

    enum class CameraMode { FirstPerson, ThirdPerson };
    CameraMode cameraMode = CameraMode::FirstPerson;
    float thirdPersonDistance = 3.0f;
    float yaw = 0.0f; 
    float pitch = 0.0f; 
    const float pitchLimit = 89.0f; 

    std::shared_ptr<Camera> cam;

    void InitializeGameWorld();

    int state = 0; // -1 for loss, 0 for playing and 1 for win
};
