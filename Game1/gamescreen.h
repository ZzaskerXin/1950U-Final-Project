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
    std::vector<std::shared_ptr<gameobject>> obstacleobjects;

    std::shared_ptr<DrawSystem> drawsystem;
    std::shared_ptr<ControlSystem> controlsystem;
    std::shared_ptr<CollisionSystem> collisionsystem;
    std::shared_ptr<AISystem> aisystem;

    std::shared_ptr<Camera> cam;
    std::shared_ptr<Shape> player;
    std::shared_ptr<Shape> shp;
    std::shared_ptr<Shape> ob;
    std::shared_ptr<ModelTransform> transform;
    std::shared_ptr<ModelTransform> playerTransform;
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

    double survivalTime = 0.0;
    double survivalThreshold = 25.0;

    glm::vec3 playerRespawnPoint = glm::vec3(0.0f, 0.5f, -5.0f);
    float avoidPlayerRadius = 2.0f;

    int lives = 3;

    bool gameOver = false;
    bool gameWon = false;

    double aiTimer = 0.0;
    double aiInterval = 1.5;

    void drawUI();

};
