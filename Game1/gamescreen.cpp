#include "gamescreen.h"

GameScreen ::GameScreen():
    cam(std::make_shared<Camera>()),
    drawsystem(std::make_shared<DrawSystem>()),
    controlsystem(std::make_shared<ControlSystem>()),
    collisionsystem(std::make_shared<CollisionSystem>()),
    aisystem (std::make_shared<AISystem>()),
    transform(std::make_shared<ModelTransform>()),
    playerTransform(std::make_shared<ModelTransform>()),
    playerobject(std::make_shared<gameobject>()),
    obstacleobjects()
{
    srand(time(0));

    drawsystem->setCamera(cam);
    controlsystem->setCamera(cam);
    controlsystem->setKeyStates(&keyStates);

    Global::graphics.addObj("ground", "Resources/Meshes/SnowTerrain.obj", true);

    std::vector<float> data = Global::graphics.getObjData("Resources/Meshes/SnowTerrain.obj");

    vertices.reserve(data.size() / 3);
    for (size_t i = 0; i < data.size(); i += 6) {
        vertices.push_back(data[i]);
        vertices.push_back(data[i + 1]);
        vertices.push_back(data[i + 2]);
    }

    shp = Global::graphics.getShape("quad");
    player = Global::graphics.getShape("sphere");
    ob = Global::graphics.getShape("cylinder");

    Global::graphics.addMaterial("player", "Resources/Images/daoli_.png", 1);
    Global::graphics.addMaterial("obstacle", "Resources/Images/wood.png", 1);

    transform->scale(glm::vec3(1.0f, 1.0f, 1.0f));
    playerTransform ->scale(glm::vec3(1.0f, 1.0f, 1.0f));
    playerTransform->setPos(glm::vec3(0.0f, 0.5f, -5.0f));
    playerTransform->rotate(glm::radians(-225.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    cam->setPos(glm::vec3(0.0f, 1.1f, -5.0f));

    int numObstacles = 5;
    float boundary = 4.0f;
    float minDistance = 2.0f;

    glm::vec3 playerStartPos = glm::vec3(0.0f, 0.5f, -5.0f);
    glm::vec3 forward = glm::vec3(0.0f, 0.0f, 1.0f);

    float minZOffset = 3.0f;
    float maxZOffset = 8.0f;
    float xSpread = 4.0f;

    for (int i = 0; i < numObstacles; i++) {
        std::shared_ptr<ModelTransform> obstacle = std::make_shared<ModelTransform>();
        glm::vec3 position;
        bool validPosition = false;

        while (!validPosition) {
            float zOffset = minZOffset + static_cast<float>(rand()) / RAND_MAX * (maxZOffset - minZOffset);
            float xOffset = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2 * xSpread;

            position = playerStartPos + forward * zOffset + glm::vec3(xOffset, 0.0f, 0.0f);

            validPosition = true;
            for (const auto& existingObstacle : obstacles) {
                if (glm::length(position - existingObstacle->getPos()) < minDistance) {
                    validPosition = false;
                    break;
                }
            }

            if (glm::length(position - playerStartPos) < 2.0f) {
                validPosition = false;
            }
        }

        obstacle->setPos(position);
        obstacles.push_back(obstacle);

        float dirX = (rand() % 2 == 0) ? 1.0f : -1.0f;
        float dirZ = (rand() % 2 == 0) ? 1.0f : -1.0f;
        obstacleDirections.push_back(glm::vec3(dirX, 0.0f, dirZ));

        obstacleSpeeds.push_back(1.5f + static_cast<float>(rand()) / RAND_MAX * 2.0f);
        std::shared_ptr<gameobject> obstacleObject = std::make_shared<gameobject>();

        obstacleObject->addComponent<RenderComponent>();
        obstacleObject->addComponent<TransformComponent>();

        obstacleObject->getComponent<RenderComponent>()->shape = ob;
        obstacleObject->getComponent<RenderComponent>()->material = "obstacle";
        obstacleObject->getComponent<TransformComponent>()->transform = obstacle;

        obstacleObject->addComponent<AIComponent>();
        obstacleObject->getComponent<AIComponent>()->target=playerobject;
        obstacleObject->getComponent<AIComponent>()->speed = obstacleSpeeds[i];
        obstacleObject->getComponent<AIComponent>()->velocity = obstacleDirections[i];
        aisystem->addAgent(obstacleObject);

        glm::vec3 basePos = obstacle->getPos();
        auto ai = obstacleObject->getComponent<AIComponent>();
        ai->patrolPoints = {
            basePos + glm::vec3(-2.0f, 0.0f, 0.0f),
            basePos + glm::vec3(2.0f, 0.0f, 0.0f)
        };
        ai->currentPatrolIndex = 0;
        ai->state = AIState::Patrol;
        ai->timeSinceLastSeen = 0.0f;

        obstacleobjects.push_back(obstacleObject);
    }

    playerobject->addComponent<RenderComponent>();
    playerobject->addComponent<TransformComponent>();
    playerobject->addComponent<CollisionComponent>();
    playerobject->getComponent<CollisionComponent>()->radius[0] = 0.5f;
    playerobject->getComponent<CollisionComponent>()->radius[1] = 0.5f;
    playerobject->getComponent<CollisionComponent>()->radius[2] = 0.5f;
    playerobject->getComponent<RenderComponent>()->shape=player;
    playerobject->getComponent<RenderComponent>()->material = "player";
    playerobject->getComponent<TransformComponent>()->transform = playerTransform;

    drawsystem->addGameObject(playerobject);
    for(auto& obstacleobject : obstacleobjects) {
        drawsystem->addGameObject(obstacleobject);
    }

    collisionsystem->setTerrain(vertices);
    controlsystem->setPlayer(playerobject);
    controlsystem->setCollisionSystem(collisionsystem);
}

GameScreen ::~GameScreen(){}


void GameScreen::update(double deltaTime){

    if (gameOver || gameWon) {
        
        return;
    } 

    controlsystem->update(deltaTime);
    aiTimer += deltaTime;
    if (aiTimer >= aiInterval) {
        aisystem->update(static_cast<float>(deltaTime));
        aiTimer = 0.0;
    }

    glm::vec3 newPlayerPos = playerTransform->getPos();
    glm::vec3 look = cam->getLook();

    playerTransform->setPos(newPlayerPos);

    for (size_t i = 0; i < obstacles.size(); i++) {
        auto& obstacle = obstacles[i];
        glm::vec3 pos = obstacle->getPos();

        auto obj = obstacleobjects[i];

        auto ai = obj->getComponent<AIComponent>();
        pos += ai->velocity * static_cast<float>(deltaTime);

        if (pos.x < -7.0f || pos.x > 7.0f) ai->velocity.x *= -1.0f;
        if (pos.z < -7.0f || pos.z > 7.0f) ai->velocity.z *= -1.0f;

        pos.y += collisionsystem->ellipsoidCollisionResponse(pos, glm::vec3(0.5f)).y;
        obstacle->setPos(pos);
    }

    collisionGrid.clear();
    for (size_t i = 0; i < obstacles.size(); i++) {
        collisionGrid.insertObject(obstacles[i]->getPos(), i);
    }

    float collisionDistance = 1.0f;
    for (size_t i = 0; i < obstacles.size(); i++) {
        glm::vec3 posA = obstacles[i]->getPos();
        auto nearby = collisionGrid.getNearbyIndices(posA);
        for (int j : nearby) {
            if (i >= j) continue;
            glm::vec3 posB = obstacles[j]->getPos();
            float distance = glm::length(posA - posB);
            if (distance < collisionDistance) {
                glm::vec3 dir = glm::normalize(posA - posB);
                glm::vec3 reflectA = glm::reflect(obstacleobjects[i]->getComponent<AIComponent>()->velocity, dir);
                glm::vec3 reflectB = glm::reflect(obstacleobjects[j]->getComponent<AIComponent>()->velocity, dir);

                float angle = glm::radians(-15.0f + static_cast<float>(rand()) / RAND_MAX * 30.0f); //
                glm::mat4 rotA = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0));
                glm::mat4 rotB = glm::rotate(glm::mat4(1.0f), -angle, glm::vec3(0, 1, 0));
                obstacleobjects[i]->getComponent<AIComponent>()->velocity = glm::vec3(rotA * glm::vec4(reflectA, 0));
                obstacleobjects[j]->getComponent<AIComponent>()->velocity = glm::vec3(rotB * glm::vec4(reflectB, 0));

                float overlap = collisionDistance - distance;
                glm::vec3 correction = dir * (overlap * 0.5f);
                glm::vec3 A = posA + correction;
                glm::vec3 B = posB - correction;
                A.y += collisionsystem->ellipsoidCollisionResponse(A, glm::vec3(0.5f)).y;
                B.y += collisionsystem->ellipsoidCollisionResponse(B, glm::vec3(0.5f)).y;
                obstacles[i]->setPos(A);
                obstacles[j]->setPos(B);
            }
        }
    }

    for (size_t i = 0; i < obstacles.size(); ++i) {
        glm::vec3 obstaclePos = obstacles[i]->getPos();
        float distance = glm::length(playerTransform->getPos() - obstaclePos);
        if (distance < 1.0f) {
            lives--;
            if (lives <= 0) {
                gameOver = true;
                // Application::game_state=-1;
            } else {
                playerTransform = std::make_shared<ModelTransform>();
                playerTransform->setPos(glm::vec3(0.0f, 0.5f, -5.0f));
                playerTransform->scale(glm::vec3(1.0f, 1.0f, 1.0f));
                playerTransform->rotate(glm::radians(-225.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                playerobject->getComponent<TransformComponent>()->transform = playerTransform;
                cam->setPos(glm::vec3(0.0f, 1.1f, -5.0f));
                cam->setLook(glm::vec3(0, 0, 1));
                cameraDistance = 2.5f;

                glm::vec3 playerStartPos = glm::vec3(0.0f, 0.5f, -5.0f);
                glm::vec3 forward = glm::vec3(0.0f, 0.0f, 1.0f);
                float minZOffset = 3.0f, maxZOffset = 8.0f, xSpread = 4.0f;

                for (int j = 0; j < obstacles.size(); j++) {
                    glm::vec3 position;
                    bool valid = false;
                    while (!valid) {
                        float zOffset = minZOffset + static_cast<float>(rand()) / RAND_MAX * (maxZOffset - minZOffset);
                        float xOffset = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2 * xSpread;
                        position = playerStartPos + forward * zOffset + glm::vec3(xOffset, 0.0f, 0.0f);
                        valid = true;
                        for (const auto& other : obstacles) {
                            if (glm::length(position - other->getPos()) < 2.0f) {
                                valid = false;
                                break;
                            }
                        }
                    }
                    obstacles[j]->setPos(position);
                    auto ai = obstacleobjects[j]->getComponent<AIComponent>();
                    ai->speed = 1.5f + static_cast<float>(rand()) / RAND_MAX * 2.0f;
                    float dirX = (rand() % 2 == 0) ? 1.0f : -1.0f;
                    float dirZ = (rand() % 2 == 0) ? 1.0f : -1.0f;
                    ai->velocity = glm::vec3(dirX, 0.0f, dirZ);

                    glm::vec3 basePos = position;
                    ai->patrolPoints = {
                        basePos + glm::vec3(-2.0f, 0.0f, 0.0f),
                        basePos + glm::vec3(2.0f, 0.0f, 0.0f)
                    };
                    ai->currentPatrolIndex = 0;
                    ai->state = AIState::Patrol;
                    ai->timeSinceLastSeen = 0.0f;
                }
            }
        }
    }

    glm::vec3 playerPos = playerTransform->getPos();
    glm::vec3 cameraOffset = glm::normalize(glm::vec3(look.x, 0.0f, look.z)) * -cameraDistance;
    glm::vec3 camPos = playerPos + cameraOffset + glm::vec3(0.0f, 0.6f, 0.0f);
    glm::vec3 smoothCamPos = glm::mix(cam->getPos(), camPos, deltaTime * 1.25f);
    cam->setPos(smoothCamPos);

    if (!gameOver && !gameWon) {
        survivalTime += deltaTime;
        if (survivalTime >= survivalThreshold) {
            gameWon = true;
        }
    }
}

void GameScreen::drawUI() {
    Global::graphics.bindShader("text");

    auto screenW = Global::graphics.getFramebufferSize().x;
    auto screenH = Global::graphics.getFramebufferSize().y;

    // === 1. Bottom Left: Hearts ===
    for (int i = 0; i < 3; ++i) {
        bool filled = (i < lives);
        if(!filled) break;
        Global::graphics.drawUIText(
            Global::graphics.getFont("DejaVuSans"),
            filled ? "♥" : " ",
            glm::ivec2(20 + i * 80, 40),
            AnchorPoint::TopLeft,
            screenW,
            1.8f,
            0.1f,
            glm::vec3(1.0f, 0.0f, 0.0f)
        );
    }

    // === 2. Bottom Right: Progress Bar ===
    int filled = static_cast<int>((survivalTime / survivalThreshold) * 10.0f);
    filled = glm::clamp(filled, 0, 10);
    std::string progressText;
    for (int i = 0; i < filled; ++i) progressText += "█";
    for (int i = filled; i < 10; ++i) progressText += "░";

    Global::graphics.drawUIText(
        Global::graphics.getFont("DejaVuSans"),
        progressText,
        glm::ivec2(screenW - 420, 40),
        AnchorPoint::TopLeft,
        screenW,
        1.2f,
        0.1f,
        glm::vec3(0.0f, 1.0f, 1.0f)
    );

    // === 3. Top Right: Score display ===
    Global::graphics.drawUIText(
        Global::graphics.getFont("DejaVuSans"),
        std::to_string(static_cast<int>(survivalTime)),  // fake score
        glm::ivec2(screenW - 200, screenH - 60),
        AnchorPoint::TopLeft,
        screenW,
        1.3f,
        0.1f,
        glm::vec3(1.0f, 1.0f, 0.0f)
    );

    // === 4. Top Left: Level label ===
    Global::graphics.drawUIText(
        Global::graphics.getFont("DejaVuSans"),
        "Level 2: Survive 25s among TUNG TUNG TUNG",
        glm::ivec2(20, screenH - 60),
        AnchorPoint::TopLeft,
        screenW,
        1.2f,
        0.1f,
        glm::vec3(0.2f, 0.2f, 0.2f)
    );
}


void GameScreen::draw(){
    drawsystem->render();

    transform->setPos(glm::vec3(0.0f, 0.0f, 0.0f));
    Global::graphics.drawObj("ground", transform->getModelMatrix());

    drawUI();


    if (gameOver) {
        Global::graphics.clearScreen(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Global::graphics.bindShader("text");
        Global::graphics.drawUIText(Global::graphics.getFont("DejaVuSans"), "Game Over! You failed to beat his dark plot!", glm::ivec2(130, 350), AnchorPoint::TopLeft, Global::graphics.getFramebufferSize().x, 0.5f, 0.1f, glm::vec3(1, 0, 0));
    }
    if (gameWon) {
        Global::graphics.clearScreen(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Global::graphics.bindShader("text");
        Global::graphics.drawUIText(Global::graphics.getFont("DejaVuSans"), "You survived 25 seconds! You have shown your power and beaten his dark plot!", glm::ivec2(130, 350), AnchorPoint::TopLeft, Global::graphics.getFramebufferSize().x, 0.5f, 0.1f, glm::vec3(1, 0, 0));
    }

    // std::string livesText = "Lives: " + std::to_string(lives);
    // Global::graphics.bindShader("text");
    // Global::graphics.drawUIText(
    //     Global::graphics.getFont("DejaVuSans"),
    //     livesText,
    //     glm::ivec2(20, 30),
    //     AnchorPoint::TopLeft,
    //     Global::graphics.getFramebufferSize().x,
    //     0.5f,
    //     0.1f,
    //     glm::vec3(1.0f, 1.0f, 1.0f)
    //     );

}

void GameScreen::keyEvent(int key, int action){
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_R) {
            resetGame();
            return;
        }
        keyStates[key] = true;
    } else if (action == GLFW_RELEASE) {
        keyStates[key] = false;
    }
}

void GameScreen::mousePosEvent(double xpos, double ypos){
    if (rightMousePressed) {
        glm::vec2 currentMousePos(xpos, ypos);
        glm::vec2 deltaMousePos = currentMousePos - prevMousePos;

        float sensitivity = 0.001f;
        sensitivity = glm::clamp(sensitivity, 0.0005f, 0.005f);


        cam->rotate(-deltaMousePos.x * sensitivity, glm::vec3(0.0f, 1.0f, 0.0f));


        glm::vec3 look = cam->getLook();


        cam->rotate(deltaMousePos.y * sensitivity, glm::vec3(look.z, 0.0f, -look.x));
        playerTransform->rotate(-deltaMousePos.x * sensitivity, glm::vec3(0.0f, 1.0f, 0.0f));


        prevMousePos = currentMousePos;
    } else {
        prevMousePos = glm::vec2(xpos, ypos);
    }
}

void GameScreen::mouseButtonEvent(int button, int action){
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            rightMousePressed = true;
        } else if (action == GLFW_RELEASE) {
            rightMousePressed = false;
        }
    }
}

void GameScreen::scrollEvent(double distance){
    cameraDistance -= (float)distance * 0.1f;
    cameraDistance = glm::clamp(cameraDistance, minDistance, maxDistance);
}

void GameScreen::resetGame() {
    std::cout << "Game reset!\n";

    playerTransform = std::make_shared<ModelTransform>();
    playerTransform->setPos(glm::vec3(0.0f, 0.5f, -5.0f));
    playerTransform->scale(glm::vec3(1.0f, 1.0f, 1.0f));
    playerTransform->rotate(glm::radians(-225.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    playerobject->getComponent<TransformComponent>()->transform = playerTransform;

    cameraDistance = 2.5f;
    cam->setPos(glm::vec3(0.0f, 1.1f, -5.0f));
    cam->setLook(glm::vec3(0, 0, 1));

    glm::vec3 playerStartPos = glm::vec3(0.0f, 0.5f, -5.0f);
    glm::vec3 forward = glm::vec3(0.0f, 0.0f, 1.0f);
    float minDistance = 2.0f;
    float minZOffset = 3.0f;
    float maxZOffset = 8.0f;
    float xSpread = 4.0f;

    for (size_t i = 0; i < obstacles.size(); ++i) {
        glm::vec3 position;
        bool valid = false;

        while (!valid) {
            float zOffset = minZOffset + static_cast<float>(rand()) / RAND_MAX * (maxZOffset - minZOffset);
            float xOffset = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2 * xSpread;

            position = playerStartPos + forward * zOffset + glm::vec3(xOffset, 0.0f, 0.0f);

            valid = true;
            for (const auto& other : obstacles) {
                if (glm::length(position - other->getPos()) < minDistance) {
                    valid = false;
                    break;
                }
            }
        }

        obstacles[i]->setPos(position);

        auto ai = obstacleobjects[i]->getComponent<AIComponent>();
        ai->speed = 1.5f + static_cast<float>(rand()) / RAND_MAX * 2.0f;
        float dirX = (rand() % 2 == 0) ? 1.0f : -1.0f;
        float dirZ = (rand() % 2 == 0) ? 1.0f : -1.0f;
        ai->velocity = glm::vec3(dirX, 0.0f, dirZ);

        glm::vec3 basePos = position;
        ai->patrolPoints = {
            basePos + glm::vec3(-2.0f, 0.0f, 0.0f),
            basePos + glm::vec3( 2.0f, 0.0f, 0.0f)
        };
        ai->currentPatrolIndex = 0;
        ai->state = AIState::Patrol;
        ai->timeSinceLastSeen = 0.0f;
    }

    survivalTime = 0.0;
    gameOver = false;
    gameWon = false;
    lives = 3;
}

void GameScreen::framebufferResizeEvent(int width, int height){
    Global::graphics.setFramebufferSize(glm::ivec2(width, height));
}

void GameScreen::windowResizeEvent(int width, int height){
    Global::graphics.setWindowSize(glm::ivec2(width, height));
    cam->resize(width, height);
}
