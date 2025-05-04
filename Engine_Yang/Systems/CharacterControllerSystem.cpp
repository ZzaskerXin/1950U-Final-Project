#include "CharacterControllerSystem.h"
#include "../GameWorld.h"
#include "CameraSystem.h"

void CharacterControllerSystem::Update(float deltaTime) {
    if (!gameWorld) return; 
    auto& inputMap = gameWorld->GetInputMap();  

    glm::vec3 cameraLook = glm::vec3(0.0f, 0.0f, 0.0f);  // Default to (0, 0, 0)

    // Directly retrieve the camera from the GameWorld
    auto cameraSystem = gameWorld->GetSystem<CameraSystem>();
    if (cameraSystem) {
        cameraLook = cameraSystem->GetCamera()->getLook();  // Fetch the camera's look direction from CameraSystem
    }

    for (auto& obj : objects) {
        auto* transform = obj->GetComponent<TransformComponent_Yang>();
        auto* physics = obj->GetComponent<PhysicsComponent>();

        if (!transform || !physics) continue;

        glm::vec3 movement(0.0f);
        float speed = 3.0f;

        if (inputMap.IsKeyPressed(GLFW_KEY_W)) movement += glm::vec3(cameraLook.x, 0.0f, cameraLook.z);
        if (inputMap.IsKeyPressed(GLFW_KEY_S)) movement -= glm::vec3(cameraLook.x, 0.0f, cameraLook.z);
        if (inputMap.IsKeyPressed(GLFW_KEY_A)) movement += glm::vec3(cameraLook.z, 0.0f, -cameraLook.x);
        if (inputMap.IsKeyPressed(GLFW_KEY_D)) movement -= glm::vec3(cameraLook.z, 0.0f, -cameraLook.x);

        if (glm::length(movement) > 0.0f) {
            movement = glm::normalize(movement) * speed * deltaTime;
        }

        transform->transform.translate(movement);

        if (!physics->isGrounded) {
            physics->velocity.y += physics->gravity * deltaTime;
        }

        if (inputMap.IsKeyPressed(GLFW_KEY_SPACE) && physics->isGrounded) {
            physics->velocity.y = physics->jumpForce;
            physics->isGrounded = false;
        }

        transform->transform.translate(physics->velocity * deltaTime);

        // if (transform->transform.getPos().y <= 0.0f) {
        //     transform->transform.setPos(glm::vec3(transform->transform.getPos().x, 0.0f, transform->transform.getPos().z));
        //     physics->velocity.y = 0;
        //     physics->isGrounded = true;
        // }
    }
}
