#include "gamescreen.h"

GameScreen ::GameScreen():
    cam(std::make_shared<Camera>()),
    drawsystem(std::make_shared<DrawSystem>()),
    controlsystem(std::make_shared<ControlSystem>()),
    collisionsystem(std::make_shared<CollisionSystem>()),
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
    player = Global::graphics.getShape("cube");
    ob = Global::graphics.getShape("cylinder");

    Global::graphics.addMaterial("player", "Resources/Images/brick.png", 1);
    Global::graphics.addMaterial("obstacle", "Resources/Images/wood.png", 1);

    transform->scale(glm::vec3(1.0f, 1.0f, 1.0f));
    playerTransform ->scale(glm::vec3(1.0f, 1.0f, 1.0f));
    playerTransform->setPos(glm::vec3(0.0f, 0.5f, -5.0f));
    cam->setPos(glm::vec3(0.0f, 1.1f, -5.0f));

    int numObstacles = 5;
    float boundary = 4.0f;
    float minDistance = 2.0f;

    for (int i = 0; i < numObstacles; i++) {
        std::shared_ptr<ModelTransform> obstacle = std::make_shared<ModelTransform>();
        glm::vec3 position;
        bool validPosition = false;

        while (!validPosition) {
            float x = -boundary + static_cast<float>(rand()) / RAND_MAX * (2 * boundary);
            float z = -boundary + static_cast<float>(rand()) / RAND_MAX * (2 * boundary);
            position = glm::vec3(x, 0.5f, z);

            validPosition = true;
            for (const auto& existingObstacle : obstacles) {
                if (glm::length(position - existingObstacle->getPos()) < minDistance) {
                    validPosition = false;
                    break;
                }
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

    if (gameOver || gameWon) return;

    controlsystem->update(deltaTime);

    glm::vec3 newPlayerPos = playerTransform->getPos();
    glm::vec3 look = cam->getLook();

    playerTransform->setPos(newPlayerPos);

    for (size_t i = 0; i < obstacles.size(); i++) {
        auto& obstacle = obstacles[i];
        glm::vec3 pos = obstacle->getPos();

        pos += obstacleDirections[i] * obstacleSpeeds[i] * static_cast<float>(deltaTime);


        if (pos.x < -7.0f || pos.x > 7.0f) obstacleDirections[i].x *= -1;
        if (pos.z < -7.0f || pos.z > 7.0f) obstacleDirections[i].z *= -1;

        pos.y += collisionsystem->ellipsoidCollisionResponse(pos, glm::vec3(0.5f, 0.5f, 0.5f)).y;

        obstacle->setPos(pos);
    }

    int totalPotentialPairs = 0;
    int skippedPairs = 0;

    collisionGrid.clear();
    for (size_t i = 0; i < obstacles.size(); i++) {
        collisionGrid.insertObject(obstacles[i]->getPos(), i);
    }

    float collisionDistance = 1.0f;

    for (size_t i = 0; i < obstacles.size(); i++) {
        glm::vec3 posA = obstacles[i]->getPos();
        auto nearby = collisionGrid.getNearbyIndices(posA);
        totalPotentialPairs += (int)nearby.size();

        for (int j : nearby) {
            if (i >= j) {
                skippedPairs++;
                continue;
            }

            glm::vec3 posB = obstacles[j]->getPos();
            float distance = glm::length(posA - posB);
            if (distance < collisionDistance) {
                glm::vec3 dir = glm::normalize(posA - posB);
                glm::vec3 reflectA = glm::reflect(obstacleDirections[i], dir);
                glm::vec3 reflectB = glm::reflect(obstacleDirections[j], dir);

                float randomAngle = (rand() % 30 - 15) * (3.14159f / 180.0f);
                glm::mat4 rotationA = glm::rotate(glm::mat4(1.0f), randomAngle, glm::vec3(0, 1, 0));
                glm::mat4 rotationB = glm::rotate(glm::mat4(1.0f), -randomAngle, glm::vec3(0, 1, 0));

                obstacleDirections[i] = glm::vec3(rotationA * glm::vec4(reflectA, 0.0f));
                obstacleDirections[j] = glm::vec3(rotationB * glm::vec4(reflectB, 0.0f));

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
            }
            else {
                cam->setPos(glm::vec3(0.0f, 1.1f, -5.0f));
                cam->setLook(glm::vec3(0, 0, 1));
                cameraDistance = 2.5f;

                float boundary = 4.0f;
                float minDistance = 1.5f;
                for (size_t j = 0; j < obstacles.size(); j++) {
                    glm::vec3 position;
                    bool valid = false;

                    while (!valid) {
                        float x = -boundary + static_cast<float>(rand()) / RAND_MAX * (2 * boundary);
                        float z = -boundary + static_cast<float>(rand()) / RAND_MAX * (2 * boundary);
                        position = glm::vec3(x, 0.5f, z);

                        valid = true;
                        for (size_t k = 0; k < j; ++k) {
                            if (glm::length(position - obstacles[k]->getPos()) < minDistance) {
                                valid = false;
                                break;
                            }
                        }
                    }

                    obstacles[j]->setPos(position);

                    float dirX = (rand() % 2 == 0) ? 1.0f : -1.0f;
                    float dirZ = (rand() % 2 == 0) ? 1.0f : -1.0f;
                    obstacleDirections[j] = glm::vec3(dirX, 0.0f, dirZ);
                    obstacleSpeeds[j] = 1.5f + static_cast<float>(rand()) / RAND_MAX * 2.0f;
                }
                playerTransform->setPos(glm::vec3(0.0f, 0.5f, -5.0f));
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

void GameScreen::draw(){
    drawsystem->render();

    transform->setPos(glm::vec3(0.0f, 0.0f, 0.0f));
    Global::graphics.drawObj("ground", transform->getModelMatrix());

    if (gameOver) {
        Global::graphics.bindShader("text");
        Global::graphics.drawUIText(Global::graphics.getFont("opensans"), "Game Over! Press R to Restart", glm::ivec2(130, 350), AnchorPoint::TopLeft, Global::graphics.getFramebufferSize().x, 0.5f, 0.1f, glm::vec3(1, 0, 0));
    }
    if (gameWon) {
        Global::graphics.bindShader("text");
        Global::graphics.drawUIText(Global::graphics.getFont("opensans"), "You survived 5 seconds! You win!", glm::ivec2(130, 350), AnchorPoint::TopLeft, Global::graphics.getFramebufferSize().x, 0.5f, 0.1f, glm::vec3(1, 0, 0));
    }

    std::string livesText = "Lives: " + std::to_string(lives);
    Global::graphics.bindShader("text");
    Global::graphics.drawUIText(
        Global::graphics.getFont("opensans"),
        livesText,
        glm::ivec2(20, 30),
        AnchorPoint::TopLeft,
        Global::graphics.getFramebufferSize().x,
        0.5f,
        0.1f,
        glm::vec3(1.0f, 1.0f, 1.0f)
        );
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
    playerTransform->setPos(glm::vec3(0.0f, 0.5f, -5.0f));

    cameraDistance = 2.5f;
    cam->setPos(glm::vec3(0.0f, 1.1f, -5.0f));
    cam->setLook(glm::vec3(0, 0, 1));

    float boundary = 4.0f;
    float minDistance = 1.5f;

    for (size_t i = 0; i < obstacles.size(); i++) {
        glm::vec3 position;
        bool validPosition = false;

        while (!validPosition) {
            float x = -boundary + static_cast<float>(rand()) / RAND_MAX * (2 * boundary);
            float z = -boundary + static_cast<float>(rand()) / RAND_MAX * (2 * boundary);
            position = glm::vec3(x, 0.5f, z);

            validPosition = true;
            for (size_t j = 0; j < i; j++) {
                if (glm::length(position - obstacles[j]->getPos()) < minDistance) {
                    validPosition = false;
                    break;
                }
            }
        }

        obstacles[i]->setPos(position);

        float dirX = (rand() % 2 == 0) ? 1.0f : -1.0f;
        float dirZ = (rand() % 2 == 0) ? 1.0f : -1.0f;
        obstacleDirections[i] = glm::vec3(dirX, 0.0f, dirZ);
        obstacleSpeeds[i] = 1.5f + static_cast<float>(rand()) / RAND_MAX * 2.0f;
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
