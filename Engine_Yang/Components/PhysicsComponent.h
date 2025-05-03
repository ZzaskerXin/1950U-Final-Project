#pragma once
#include "Component.h"
#include "glm/glm.hpp"
#include <vector>

class PhysicsComponent : public Component {
public:
    glm::vec3 velocity = glm::vec3(0.0f);
    float gravity = -9.8f;
    float jumpForce = 5.0f;
    bool isGrounded = true;

    float radius = 0.5f;
    float height = 2.0f;
    float mass = 1.0f;
    bool isStatic = false;

    std::vector<glm::vec3> convexHull;  // Store Convex Hull

    PhysicsComponent(float r = 0.5f, float h = 1.0f) 
        : radius(r), height(h) {}

    void SetConvexHull(const std::vector<glm::vec3>& hull) {
        convexHull = hull;
    }

    const std::vector<glm::vec3>& GetConvexHull() const {
        return convexHull;
    }
};