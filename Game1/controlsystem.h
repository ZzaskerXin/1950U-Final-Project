#include "Engine/gameobject.h"
#include "Engine/system.h"
#include "collisionsystem.h"

class ControlSystem : public System{
public:

    void setCollisionSystem(std::shared_ptr<CollisionSystem> collSys) {
        collisionSystem = collSys;
    }

    void setPlayer(std::shared_ptr<gameobject> playerObj) {
        player = playerObj;
    }

    void setCamera(std::shared_ptr<Camera> cam) {
        camera = cam;
    }

    void setKeyStates(const std::unordered_map<int, bool>* keys) {
        keyStates = keys;
    }

    void update(double deltaTime) {
        auto transform = player->getComponent<TransformComponent>()->transform;
        glm::vec3 look = camera->getLook();
        glm::vec3 movement(0.0f);

        float speed = 5.0f * deltaTime;

        if (keyStates->count(GLFW_KEY_W) && keyStates->at(GLFW_KEY_W)) {
            movement += glm::vec3(look.x, 0.0f, look.z);
        }
        if (keyStates->count(GLFW_KEY_S) && keyStates->at(GLFW_KEY_S)) {
            movement -= glm::vec3(look.x, 0.0f, look.z);
        }
        if (keyStates->count(GLFW_KEY_A) && keyStates->at(GLFW_KEY_A)) {
            movement += glm::vec3(look.z, 0.0f, -look.x);
        }
        if (keyStates->count(GLFW_KEY_D) && keyStates->at(GLFW_KEY_D)) {
            movement -= glm::vec3(look.z, 0.0f, -look.x);
        }

        if (glm::length(movement) > 0.0f) {
            movement = glm::normalize(movement) * speed;
        }

        if (keyStates->count(GLFW_KEY_SPACE) && keyStates->at(GLFW_KEY_SPACE) && !isJumping) {
            isJumping = true;
            jumpVelocity = jumpForce;
        }

        jumpVelocity -= gravity * deltaTime;
        movement.y += jumpVelocity * deltaTime;

        glm::vec3 newPos = transform->getPos() += movement;

        if(collisionSystem->isCollidingWithGround(newPos, player->getComponent<CollisionComponent>()->radius.y)) {
            isJumping = false;
            jumpVelocity = 0.0f;
        }

        if (!isJumping) {
            glm::vec3 mtv = collisionSystem->ellipsoidCollisionResponse(newPos, player->getComponent<CollisionComponent>()->radius);
            newPos.y += mtv.y;
        }

        float boundary = 5.0f;
        newPos.x = glm::clamp(newPos.x, -boundary, boundary);
        newPos.z = glm::clamp(newPos.z, -boundary, boundary);
        transform->setPos(newPos);
    }

private:
    std::shared_ptr<Camera> camera;
    std::shared_ptr<gameobject> player;
    const std::unordered_map<int, bool>* keyStates;
    std::shared_ptr<CollisionSystem> collisionSystem;

    bool isJumping = false;
    float jumpVelocity = 0.0f;
    const float gravity = 9.8f;
    const float jumpForce = 5.0f;
};
