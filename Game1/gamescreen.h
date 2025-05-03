#pragma once

#include "Engine/UniformGrid.h"
#include "Engine/screen.h"
#include <cstdlib>
#include <ctime>
#include "drawsystem.h"
#include "controlsystem.h"
#include "aisystem.h"

class GameScreen : public Screen
{
public:
    GameScreen();
    ~GameScreen();

    void update(double deltaTime);
    void draw();
    void keyEvent(int key, int action);
    void mousePosEvent(double xpos, double ypos);
    void mouseButtonEvent(int button, int action);
    void scrollEvent(double distance);
    void windowResizeEvent(int width, int height);
    void framebufferResizeEvent(int width, int height);
    void resetGame();
private:
    std::shared_ptr<gameobject> playerobject;
    std::shared_ptr<gameobject>targetobject;
    std::vector<std::shared_ptr<gameobject>> obstacleobjects;

    std::shared_ptr<DrawSystem> drawsystem;
    std::shared_ptr<ControlSystem> controlsystem;
    std::shared_ptr<CollisionSystem> collisionsystem;
    std::shared_ptr<AISystem> aisystem;

    std::shared_ptr<Camera> cam;
    std::shared_ptr<Shape> player;
    std::shared_ptr<Shape> shp;
    std::shared_ptr<Shape> target;
    std::shared_ptr<Shape> ob;
    std::shared_ptr<ModelTransform> transform;
    std::shared_ptr<ModelTransform> playerTransform;
    std::shared_ptr<ModelTransform> targetTransform;
    std::vector<std::shared_ptr<ModelTransform>> obstacles;
    std::vector<glm::vec3> obstacleDirections;
    std::vector<float> obstacleSpeeds;

    std::unordered_map<int, bool> keyStates;

    std::vector<float> vertices;
    UniformGrid collisionGrid;

    bool rightMousePressed = false;
    glm::vec2 prevMousePos;


    float cameraDistance = 2.5f;
    const float minDistance = 0.0f;
    const float maxDistance = 5.0f;

    bool gameOver = false;
    bool gameWon = false;
    bool playerAtTarget = false;

    double timeSinceLastRepath = 0.0;
};
