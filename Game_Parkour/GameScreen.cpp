#include "GameScreen.h"
#include <iostream>
#include <random>
#include <algorithm>

std::vector<glm::vec3> ReduceConvexHull(const std::vector<glm::vec3>& points, int targetVertices) {
    if (points.size() <= targetVertices) return points;

    std::vector<glm::vec3> reducedHull = points;
    std::shuffle(reducedHull.begin(), reducedHull.end(), std::mt19937{std::random_device{}()});
    reducedHull.resize(targetVertices);
    
    return reducedHull;
}


std::shared_ptr<GameObject> CreateBlock(const glm::vec3& pos, 
    const std::string& shape = "cube", 
    const std::string& texture = "wood.png", 
    bool isStatic = true, 
    float size = 0.5f, 
    float mass = 1.0f, 
    int id = 8888) {
    auto block = std::make_shared<GameObject>(id);
    block->AddComponent<TransformComponent>();
    block->GetComponent<TransformComponent>()->transform.setPos(pos);
    block->AddComponent<MeshComponent>("primitive", shape, texture);
    auto physics = block->AddComponent<PhysicsComponent>(size, mass);

    std::vector<glm::vec3> convexHull = {
        glm::vec3(-size, -size, -size),
        glm::vec3(size, -size, -size),
        glm::vec3(size, size, -size),
        glm::vec3(-size, size, -size),
        glm::vec3(-size, -size, size),
        glm::vec3(size, -size, size),
        glm::vec3(size, size, size),
        glm::vec3(-size, size, size)
    };
    physics->SetConvexHull(convexHull);

    if (isStatic) {
        physics->isStatic = true;
    }

    return block;
}

std::shared_ptr<GameObject> CreateObjObject(const glm::vec3& pos, 
    const std::string& shapeName, 
    const std::string& shapePath,
    const std::string& texture="",
    bool isStatic = true, 
    float size = 1.0f, 
    float mass = 1.0f, 
    int id = 9999) {

    auto obj = std::make_shared<GameObject>(id);
    obj->AddComponent<TransformComponent>();
    obj->GetComponent<TransformComponent>()->transform.setPos(pos);
    
    obj->AddComponent<MeshComponent>("custom", shapeName, texture);

    auto physics = obj->AddComponent<PhysicsComponent>(size, mass);

    std::vector<glm::vec3> convexHull = Global::graphics.addShape(shapeName, shapePath, true);
    std::vector<glm::vec3> simplifiedHull = ReduceConvexHull(convexHull, 100); // Reduce to 100 vertices
    physics->SetConvexHull(simplifiedHull);

    // if (!isStatic) {
    //   obj->AddComponent<PhysicsComponent>(size, mass);
    // }
    return obj;
}


GameScreen_Parkour::GameScreen_Parkour(std::shared_ptr<ScreenManager> manager) : screenManager(manager) {
    std::cout << "Starting GameScreen_Parkour" << std::endl;
    std::cout << screenManager.get() << std::endl;
    
    InitializeGameWorld();
}

void GameScreen_Parkour::InitializeGameWorld() {
    // std::cout << "Starting GameScreen_Parkour init" << std::endl;
    // std::cout << "Starting GameScreen_Parkour gameworld" << std::endl;


    // Global::graphics.addMaterial("default", "Resources/Images/wood.png", 0.5);

    gameWorld = std::make_shared<GameWorld>();
    // std::cout << "Starting GameScreen_Parkour player" << std::endl;

    auto player = std::make_shared<GameObject>(1001);

    // std::cout << "Player created" << std::endl;

    player->AddComponent<TransformComponent>();
    player->GetComponent<TransformComponent>()->transform.setPos(glm::vec3(0, 3.0f, 0));

    // std::cout << "transform created" << std::endl;

    player->AddComponent<PhysicsComponent>(0.0f, 0.5f);
    // player->GetComponent<PhysicsComponent>()->isStatic = true;

    
    // std::cout << "phy created" << std::endl;

    player->AddComponent<MeshComponent>("primitive", "sphere", "daoli_.png");
    float size = 0.2f;
    std::vector<glm::vec3> convexHull_test = {
        glm::vec3(-size, -size, -size),
        glm::vec3(size, -size, -size),
        glm::vec3(size, size, -size),
        glm::vec3(-size, size, -size),
        glm::vec3(-size, -size, size),
        glm::vec3(size, -size, size),
        glm::vec3(size, size, size),
        glm::vec3(-size, size, size)
    };

    float radius = player->GetComponent<PhysicsComponent>()->radius;
    float halfRadius = radius * 0.70710678118f;  // sqrt(2)/2 approximation for diagonal directions

    std::vector<glm::vec3> sphereHull = {
        { 0,  0,  radius},  // Front
        { 0,  0, -radius},  // Back
        { 0,  radius,  0},  // Top
        { 0, -radius,  0},  // Bottom
        { radius,  0,  0},  // Right
        {-radius,  0,  0},  // Left

        // Diagonal points (octahedral expansion)
        { halfRadius,  halfRadius,  halfRadius},   // Upper Front Right
        {-halfRadius,  halfRadius,  halfRadius},   // Upper Front Left
        { halfRadius, -halfRadius,  halfRadius},   // Lower Front Right
        {-halfRadius, -halfRadius,  halfRadius},   // Lower Front Left
        { halfRadius,  halfRadius, -halfRadius},   // Upper Back Right
        {-halfRadius,  halfRadius, -halfRadius},   // Upper Back Left
        { halfRadius, -halfRadius, -halfRadius},   // Lower Back Right
        {-halfRadius, -halfRadius, -halfRadius}    // Lower Back Left
    };

    
    // 🔹 Set Convex Hull in Local Space
    player->GetComponent<PhysicsComponent>()->SetConvexHull(sphereHull);
    
    std::cout << "mesh created for shapes" << std::endl;
    gameWorld->AddGameObject(player);


    std::cout << "Starting GameScreen_Parkour grass land" << std::endl;

    int gridSize = 2;
    for (int x = -gridSize; x <= gridSize; ++x) { 
        for (int z = -gridSize; z <= gridSize; ++z) {
            auto grassTile = CreateBlock(glm::vec3(x, 0.0f, z), "cube", "grass.png", true);
            gameWorld->AddGameObject(grassTile);
    
            if (x * x == gridSize * gridSize || z * z == gridSize * gridSize) {
                auto brickTile = CreateBlock(glm::vec3(x, 1.0f, z), "cube", "brick.png", true);
                gameWorld->AddGameObject(brickTile);
            }
        }
    }

    // gameWorld->AddGameObject(CreateBlock(glm::vec3(0, 1, 2)));  // Default cube with "diamond.png"
    // gameWorld->AddGameObject(CreateBlock(glm::vec3(-2, 0, 2)));  // Static block

    gameWorld->AddGameObject(CreateBlock(glm::vec3(0, 0, 2)));  // First jump
    gameWorld->AddGameObject(CreateBlock(glm::vec3(1, 1, 4)));  // Slightly farther
    gameWorld->AddGameObject(CreateBlock(glm::vec3(-1, 2, 6))); // Introduces height variation
    gameWorld->AddGameObject(CreateBlock(glm::vec3(2, 2, 8)));  // Side jump
    gameWorld->AddGameObject(CreateBlock(glm::vec3(0, 3, 10))); // Central stability block

    // Tower climb challenge
    gameWorld->AddGameObject(CreateBlock(glm::vec3(0, 4, 11)));
    gameWorld->AddGameObject(CreateBlock(glm::vec3(0, 5, 12)));
    gameWorld->AddGameObject(CreateBlock(glm::vec3(0, 6, 13)));  // Top of tower

    // Final jump (long distance)
    gameWorld->AddGameObject(CreateBlock(glm::vec3(2, 6, 16), "cube", "diamond.png", true));  // Goal block with different texture




    std::cout << "Starting GameScreen_Parkour camera" << std::endl;


    auto cameraSystem = std::make_shared<CameraSystem>();
    cameraSystem->AddObject(player);  
    gameWorld->AddSystem(cameraSystem);

    std::cout << "Starting GameScreen_Parkour controller" << std::endl;


    auto characterController = std::make_shared<CharacterControllerSystem>();
    characterController->AddObject(player);
    

    std::cout << "Starting GameScreen_Parkour draw" << std::endl;


    auto drawSystem = std::make_shared<DrawSystem>("phong");
    gameWorld->AddSystem(drawSystem);

    auto collisionSystem = std::make_shared<CollisionSystem>();
    gameWorld->AddSystem(collisionSystem);
    gameWorld->AddSystem(characterController);

    std::string egg_shape = "egg";
    std::string egg_obj_path = "Resources/Meshes/12954_multi_color_Easter_egg_v2_l3.obj";

    Global::graphics.addObj(egg_shape, egg_obj_path, true);
    gameWorld->AddGameObject(CreateObjObject(glm::vec3(6, 0, 0), egg_shape, egg_obj_path));


    std::cout<< "finish GameScreen_Parkour initializing!" << std::endl;

        // for debug
    //     std::cout << "Paused... Press Enter to continue." << std::endl;
    // std::cin.get();

}

void GameScreen_Parkour::update(double deltaTime) {
    // if(screenManager->GetCurrentScreen().get() != this) return;

    // static double timer = 0.0;
    // timer += deltaTime;
    auto player = gameWorld->GetObjectById(1001);
    if (!player) return;
    auto* transform = player->GetComponent<TransformComponent>();
    if (!transform) return;

    glm::vec3 playerPos = transform->transform.getPos();

    gameWorld->Update(deltaTime);

    
    glm::vec3 winPos = glm::vec3(2, 7, 16);
    float winThreshold = 0.5f;

    if (glm::distance(playerPos, winPos) < winThreshold) {  
        state = 1;
    }
    
    if (playerPos.y < -5.0f) {  
        state = -1; 
    }
    // std::cout<< state<< std::endl;
    // std::cout<< playerPos.x << " " <<  playerPos.y<<  " " <<  playerPos.z <<std::endl;
    if(state == 1) Game::switchToScreen("WIN");
    if(state == -1) Game::switchToScreen("LOSS"); 
}

void GameScreen_Parkour::draw() {
    if(screenManager->GetCurrentScreen().get() != this) return;

    gameWorld->Draw();
    
}

void GameScreen_Parkour::keyEvent(int key, int action) {
    if(screenManager->GetCurrentScreen().get() != this) return;

    bool isPressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
    gameWorld->GetInputMap().SetKeyState(key, isPressed);
}

void GameScreen_Parkour::mousePosEvent(double xpos, double ypos) {
    auto cameraSystem = gameWorld->GetSystem<CameraSystem>();
    if (cameraSystem) {
        cameraSystem->mousePosEvent(xpos, ypos);
    }
}

void GameScreen_Parkour::mouseButtonEvent(int button, int action) {
    // std::cout << "mouse button evenet!" << std::endl;
    auto cameraSystem = gameWorld->GetSystem<CameraSystem>();
    if (cameraSystem) {
        cameraSystem->mouseButtonEvent(button, action);
    }
}

void GameScreen_Parkour::scrollEvent(double distance) {
    auto cameraSystem = gameWorld->GetSystem<CameraSystem>();
    if (cameraSystem) {
        cameraSystem->scrollEvent(distance);
    }
}

void GameScreen_Parkour::framebufferResizeEvent(int width, int height) {
    auto cameraSystem = gameWorld->GetSystem<CameraSystem>();
    if (cameraSystem) {
        cameraSystem->framebufferResizeEvent(width, height);
    }
}

void GameScreen_Parkour::windowResizeEvent(int width, int height) {
    auto cameraSystem = gameWorld->GetSystem<CameraSystem>();
    if (cameraSystem) {
        cameraSystem->windowResizeEvent(width, height);
    }
}

