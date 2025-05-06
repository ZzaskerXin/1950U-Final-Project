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


std::shared_ptr<GameObject_Yang> CreateBlock(const glm::vec3& pos, 
    const std::string& shape = "cube", 
    const std::string& texture = "wood.png", 
    bool isStatic = true, 
    float size = 0.5f, 
    float mass = 1.0f, 
    int id = 8888) {
    auto block = std::make_shared<GameObject_Yang>(id);
    block->AddComponent<TransformComponent_Yang>();
    block->GetComponent<TransformComponent_Yang>()->transform.setPos(pos);
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

std::shared_ptr<GameObject_Yang> CreateObjObject(const glm::vec3& pos, 
    const std::string& shapeName, 
    const std::string& shapePath,
    const std::string& texture="",
    bool isStatic = true, 
    float size = 1.0f, 
    float mass = 1.0f, 
    int id = 9999) {

    auto obj = std::make_shared<GameObject_Yang>(id);
    obj->AddComponent<TransformComponent_Yang>();
    obj->GetComponent<TransformComponent_Yang>()->transform.setPos(pos);
    
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

void addUIs(std::shared_ptr<GameWorld> gameWorld, int screenW, int screenH) {

   
    // === 1. Bottom Left: 3 Hearts ===
    for (int i = 0; i < 3; ++i) {
        auto heart = std::make_shared<GameObject_Yang>(9000 + i);
        heart->AddComponent<UIComponent>("♥");
        auto comp = heart->GetComponent<UIComponent>();
        comp->position = glm::ivec2(20 + i * 40, 40); // from bottom-left
        comp->scale = 1.8f;
        comp->color = glm::vec3(1.0f, 0.0f, 0.0f); // red
        gameWorld->AddGameObject(heart);
    }

    // === 2. Bottom Right: Progress Bar (ID 9100) ===
    auto progressBar = std::make_shared<GameObject_Yang>(9100);
    progressBar->AddComponent<UIComponent>("█████░░░░░");
    auto compBar = progressBar->GetComponent<UIComponent>();
    compBar->position = glm::ivec2(screenW - 200, 40); // margin from right edge
    compBar->scale = 1.2f;
    compBar->color = glm::vec3(0.0f, 1.0f, 1.0f); // cyan
    gameWorld->AddGameObject(progressBar);

    // === 3. Top Right: Score Display (ID 9200) ===
    // auto scoreDisplay = std::make_shared<GameObject_Yang>(9200);
    // scoreDisplay->AddComponent<UIComponent>("12345");
    // auto compScore = scoreDisplay->GetComponent<UIComponent>();
    // compScore->position = glm::ivec2(screenW - 200, screenH - 60); // top-right
    // compScore->scale = 1.3f;
    // compScore->color = glm::vec3(1.0f, 1.0f, 0.0f); // yellow
    // gameWorld->AddGameObject(scoreDisplay);

    // === 4. Top Left: Level Label (ID 9300) ===
    auto label = std::make_shared<GameObject_Yang>(9300);
    label->AddComponent<UIComponent>("Level 1: Reach the diamond block");
    auto compLabel = label->GetComponent<UIComponent>();
    compLabel->position = glm::ivec2(20, screenH - 60); // top-left
    compLabel->scale = 1.2f;
    compLabel->color = glm::vec3(0.2f, 0.2f, 0.2f); // light gray
    gameWorld->AddGameObject(label);
}



void GameScreen_Parkour::InitializeGameWorld() {
    // std::cout << "Starting GameScreen_Parkour init" << std::endl;
    // std::cout << "Starting GameScreen_Parkour gameworld" << std::endl;


    // Global::graphics.addMaterial("default", "Resources/Images/wood.png", 0.5);

    gameWorld = std::make_shared<GameWorld>();
    // std::cout << "Starting GameScreen_Parkour player" << std::endl;

    // auto uiText = std::make_shared<GameObject_Yang>(9999);
    // uiText->AddComponent<UIComponent>("UI test with phong shad " );
    // gameWorld->AddGameObject(uiText);

    addUIs(gameWorld, framebufferWidth, framebufferHeight);
    for (int i = 0; i < 3; ++i)
        heartIds.push_back(9000 + i);


    auto player = std::make_shared<GameObject_Yang>(1001);

    // std::cout << "Player created" << std::endl;

    player->AddComponent<TransformComponent_Yang>();
    player->GetComponent<TransformComponent_Yang>()->transform.setPos(glm::vec3(0, 3.0f, 0));

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


    auto drawSystem = std::make_shared<DrawSystem_Yang>("combined");
    gameWorld->AddSystem(drawSystem);

    auto collisionSystem = std::make_shared<CollisionSystem_Yang>();
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
void GameScreen_Parkour::updateUI() {
    // === Update hearts ===
    for (int i = 0; i < heartIds.size(); ++i) {
        auto obj = gameWorld->GetObjectById(heartIds[i]);
        if (!obj) continue;
        auto* comp = obj->GetComponent<UIComponent>();
        if (comp) {
            comp->text = (i < currentLives) ? "♥" : " ";
            comp->position = glm::ivec2(20 + i * 80, 40); // fixed bottom-left
        }
    }
    
    // === Update progress bar (bottom right) ===
    if (auto obj = gameWorld->GetObjectById(progressBarId)) {
        auto* comp = obj->GetComponent<UIComponent>();
        if (comp) {
            int filled = static_cast<int>(progressPercent * 10);
            std::string full;
            for (int i = 0; i < filled; ++i) full += "█";
            for (int i = filled; i < 10; ++i) full += "░";
            comp->text = full;
            comp->position = glm::ivec2(framebufferWidth - 420, 40);
        }
    }

    // === Update score display (top right) ===
    if (auto obj = gameWorld->GetObjectById(scoreDisplayId)) {
        auto* comp = obj->GetComponent<UIComponent>();
        if (comp) {
            comp->text = std::to_string(score);
            comp->position = glm::ivec2(framebufferWidth - 200, framebufferHeight - 60);
        }
    }

    // === Update level label (top left) ===
    if (auto obj = gameWorld->GetObjectById(levelLabelId)) {
        auto* comp = obj->GetComponent<UIComponent>();
        if (comp) {
            comp->text = levelText;
            comp->position = glm::ivec2(20, framebufferHeight - 60);
        }
    }
}


void GameScreen_Parkour::update(double deltaTime) {
    // if(screenManager->GetCurrentScreen().get() != this) return;

    // static double timer = 0.0;
    // timer += deltaTime;

    updateUI();
    auto player = gameWorld->GetObjectById(1001);
    if (!player) return;
    auto* transform = player->GetComponent<TransformComponent_Yang>();
    if (!transform) return;

    glm::vec3 playerPos = transform->transform.getPos();

    gameWorld->Update(deltaTime);

    glm::vec3 startPos = glm::vec3(0, 3.0f, 0);     // starting point
    glm::vec3 goalPos = glm::vec3(2, 7, 16);        // final goal

    float totalDist = glm::distance(startPos, goalPos);
    float currentDist = glm::distance(startPos, playerPos);

    progressPercent = std::clamp(currentDist / totalDist, 0.0f, 1.0f);

    
    glm::vec3 winPos = glm::vec3(2, 7, 16);
    float winThreshold = 0.5f;

    if (glm::distance(playerPos, winPos) < winThreshold) {  
        state = 1;
    }
    
    if (playerPos.y < -5.0f) {  
        currentLives--;
        if (currentLives <= 0) {
            state = -1;
        } else {
            // Optionally reset player position
            transform->transform.setPos(glm::vec3(0, 3.0f, 0));  // example respawn point
            auto* physics = player->GetComponent<PhysicsComponent>();
        if (physics) {
            physics->velocity = glm::vec3(0.0f);
            // physics->angularVelocity = glm::vec3(0.0f); // if used
        }
        }
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
    framebufferWidth = width;
    framebufferHeight = height;
    auto cameraSystem = gameWorld->GetSystem<CameraSystem>();
    if (cameraSystem) {
        cameraSystem->framebufferResizeEvent(width, height);
    }
    // addUIs(gameWorld, framebufferWidth, framebufferHeight);
}

void GameScreen_Parkour::windowResizeEvent(int width, int height) {
    auto cameraSystem = gameWorld->GetSystem<CameraSystem>();
    if (cameraSystem) {
        cameraSystem->windowResizeEvent(width, height);
    }
}

