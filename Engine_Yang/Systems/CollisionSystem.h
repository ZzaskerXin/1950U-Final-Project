#pragma once
#include "System.h"
#include "GameObject.h"
#include "PhysicsComponent.h"
#include "TransformComponent.h"
#include <vector>
#include <memory>
#include "GameWorld.h"

class CollisionSystem : public System {
public:
    void Update(float deltaTime) override;

private:
    // 🔹 GJK & EPA-Based Collision Detection
    bool CheckCollision(const std::vector<glm::vec3>& shape1, 
                        const std::vector<glm::vec3>& shape2, 
                        std::vector<glm::vec3>& simplex);

    glm::vec3 ComputeMTV(std::vector<glm::vec3>& simplex, 
                            const std::vector<glm::vec3>& shape1, 
                            const std::vector<glm::vec3>& shape2);

    void ApplyCollisionResponse(GameObject* obj1, PhysicsComponent* physics1,
                                GameObject* obj2, PhysicsComponent* physics2,
                                const glm::vec3& mtv);

    // 🔹 GJK & EPA Helper Functions
    glm::vec3 Support(const std::vector<glm::vec3>& shape, const glm::vec3& direction);
    bool HandleSimplex(std::vector<glm::vec3>& simplex, glm::vec3& direction);
};