#include "gamescreen.h"

GameScreen ::GameScreen():
    cam(std::make_shared<Camera>()),
    drawsystem(std::make_shared<DrawSystem>()),
    controlsystem(std::make_shared<ControlSystem>()),
    collisionsystem(std::make_shared<CollisionSystem>()),
    aisystem(std::make_shared<AISystem>()),
    transform(std::make_shared<ModelTransform>()),
    playerTransform(std::make_shared<ModelTransform>()),
    targetTransform(std::make_shared<ModelTransform>()),
    playerobject(std::make_shared<gameobject>()),
    targetobject(std::make_shared<gameobject>()),
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
    target = Global::graphics.getShape("cube");
    ob = Global::graphics.getShape("cylinder");

    Global::graphics.addMaterial("texture", "Resources/Images/grass.png", 1);
    Global::graphics.addMaterial("player", "Resources/Images/player.png", 1);
    Global::graphics.addMaterial("target", "Resources/Images/target.png", 1);
    Global::graphics.addMaterial("obstacle", "Resources/Images/ob.jpg", 1);

    transform->scale(glm::vec3(1.0f, 1.0f, 1.0f));
    playerTransform ->scale(glm::vec3(1.0f, 1.0f, 1.0f));
    playerTransform->setPos(glm::vec3(0.0f, 0.5f, -5.0f));
    targetTransform ->scale(glm::vec3(1.0f, 1.0f, 1.0f));
    targetTransform->setPos(glm::vec3(0.0f, 0.5f, 5.0f));
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

    targetobject->addComponent<RenderComponent>();
    targetobject->addComponent<TransformComponent>();
    targetobject->getComponent<RenderComponent>()->shape=target;
    targetobject->getComponent<RenderComponent>()->material = "target";
    targetobject->getComponent<TransformComponent>()->transform = targetTransform;

    drawsystem->addGameObject(playerobject);
    for(auto& obstacleobject : obstacleobjects) {
        drawsystem->addGameObject(obstacleobject);
    }
    drawsystem->addGameObject(targetobject);

    collisionsystem->setTerrain(vertices);
    controlsystem->setPlayer(playerobject);
    controlsystem->setCollisionSystem(collisionsystem);
}

GameScreen ::~GameScreen(){}


void GameScreen::update(double deltaTime){

    if (gameOver || gameWon) return;

    controlsystem->update(deltaTime);

    for (int i = 0; i < obstacles.size(); ++i) {
        glm::vec3 pos = obstacles[i]->getPos();
        std::cout << "[Debug] Obstacle " << i << " Position: " << glm::to_string(pos) << std::endl;
    }

    glm::vec3 newPlayerPos = playerTransform->getPos();
    glm::vec3 look = cam->getLook();

    timeSinceLastRepath += deltaTime;
    if (timeSinceLastRepath >= 1.0f) {
        int idx = rand() % obstacleobjects.size();
        glm::vec3 pos = obstacleobjects[idx]->getComponent<TransformComponent>()->transform->getPos();
        aisystem->addAgent(idx, pos, playerTransform->getPos());
        timeSinceLastRepath = 0.0f;
    }

    aisystem->updateAgentDirectionsAndSpeeds(obstacleDirections, obstacleSpeeds, obstacleobjects, deltaTime);

    playerAtTarget = glm::length(newPlayerPos - targetTransform->getPos()) < 1.0f;
    if (playerAtTarget){
        std::cout << "You won! Press R to restart.\n";
        gameWon = true;
    }
    else {
        playerTransform->setPos(newPlayerPos);
    }

    for (size_t i = 0; i < obstacles.size(); i++) {
        auto& obstacle = obstacles[i];
        glm::vec3 pos = obstacle->getPos();

        pos += obstacleDirections[i] * obstacleSpeeds[i] * static_cast<float>(deltaTime);


        if (pos.x < -5.0f || pos.x > 5.0f) obstacleDirections[i].x *= -1;
        if (pos.z < -5.0f || pos.z > 5.0f) obstacleDirections[i].z *= -1;

        if (glm::length(pos - targetTransform->getPos()) < 1.0f) {
            glm::vec3 toTarget = glm::normalize(pos - targetTransform->getPos());
            glm::vec3 reflectDir = glm::reflect(obstacleDirections[i], toTarget);

            float angleOffset = (rand() % 30 - 15) * (3.14159f / 180.0f);
            glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angleOffset, glm::vec3(0, 1, 0));
            obstacleDirections[i] = glm::vec3(rotation * glm::vec4(reflectDir, 0.0f));
        }

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

    for (const auto& obstacle : obstacles) {
        if (glm::length(newPlayerPos - obstacle->getPos()) < 1.0f) {
            std::cout << "You got hit! Press R to restart.\n";
            gameOver = true;
        }
    }

    glm::vec3 playerPos = playerTransform->getPos();
    glm::vec3 cameraOffset = glm::normalize(glm::vec3(look.x, 0.0f, look.z)) * -cameraDistance;
    glm::vec3 camPos = playerPos + cameraOffset + glm::vec3(0.0f, 0.6f, 0.0f);
    glm::vec3 smoothCamPos = glm::mix(cam->getPos(), camPos, deltaTime * 1.25f);
    cam->setPos(smoothCamPos);
}

void GameScreen::draw(){
    drawsystem->render();

    transform->setPos(glm::vec3(0.0f, 0.0f, 0.0f));
    Global::graphics.drawObj("ground", transform->getModelMatrix());

    if (gameOver) {
        Global::graphics.bindShader("text");
        Global::graphics.drawUIText(Global::graphics.getFont("opensans"), "Game Over! Press R to Restart", glm::ivec2(160, 350), AnchorPoint::TopLeft, Global::graphics.getFramebufferSize().x, 0.5f, 0.1f, glm::vec3(1, 1, 1));
    }
    if (gameWon) {
        Global::graphics.bindShader("text");
        Global::graphics.drawUIText(Global::graphics.getFont("opensans"), "You Win! Press R to Restart", glm::ivec2(160, 350), AnchorPoint::TopLeft, Global::graphics.getFramebufferSize().x, 0.5f, 0.1f, glm::vec3(1, 1, 1));
    }
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

    timeSinceLastRepath = 0.0;
    aisystem->clearAgents();

    gameOver = false;
    gameWon = false;
    playerAtTarget = false;
}

void GameScreen::framebufferResizeEvent(int width, int height){
    Global::graphics.setFramebufferSize(glm::ivec2(width, height));
}

void GameScreen::windowResizeEvent(int width, int height){
    Global::graphics.setWindowSize(glm::ivec2(width, height));
    cam->resize(width, height);
}
