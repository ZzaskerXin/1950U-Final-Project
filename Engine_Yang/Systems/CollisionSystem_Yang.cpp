#include "CollisionSystem_Yang.h"
#include <iostream>
#include <limits>
#include <glm/gtc/epsilon.hpp>

// Transform convex hull to world space
std::vector<glm::vec3> TransformHull(const std::vector<glm::vec3> &hull, GameObject_Yang *obj) {
    glm::mat4 transform = obj->GetComponent<TransformComponent_Yang>()->transform.getModelMatrix();
    std::vector<glm::vec3> transformedHull;
    for (const auto &v : hull) {
        glm::vec4 worldPos = transform * glm::vec4(v, 1.0f);
        transformedHull.push_back(glm::vec3(worldPos));
    }
    return transformedHull;
}

void CollisionSystem_Yang::Update(float deltaTime) {
    auto objects = gameWorld->GetObjects();
    bool anyCollisionDetected = false; 

    std::vector<std::tuple<GameObject_Yang*, PhysicsComponent*, GameObject_Yang*, PhysicsComponent*, glm::vec3>> collisionPairs;

    for (size_t i = 0; i < objects.size(); ++i) {
        for (size_t j = i + 1; j < objects.size(); ++j) {
            auto obj1 = objects[i].get();
            auto obj2 = objects[j].get();

            auto *physics1 = obj1->GetComponent<PhysicsComponent>();
            auto *physics2 = obj2->GetComponent<PhysicsComponent>();

            if (!physics1 || !physics2 || (physics1->isStatic && physics2->isStatic))
                continue;

            std::vector<glm::vec3> shape1 = TransformHull(physics1->GetConvexHull(), obj1);
            std::vector<glm::vec3> shape2 = TransformHull(physics2->GetConvexHull(), obj2);

            if (shape1.size() < 4 || shape2.size() < 4)
                continue;

            std::vector<glm::vec3> simplex;
            if (CheckCollision(shape1, shape2, simplex)) {
                glm::vec3 mtv = ComputeMTV(simplex, shape1, shape2);
                collisionPairs.push_back({obj1, physics1, obj2, physics2, mtv});

                // std::cout <<  Collision detected between: " << "obj1" << " and " << "obj2"
                        //   << " | MTV: (" << mtv.x << ", " << mtv.y << ", " << mtv.z << ")" << std::endl;
                
                anyCollisionDetected = true;
            }
        }
    }



    // Apply MTV adjustments
    for (auto& [obj1, physics1, obj2, physics2, mtv] : collisionPairs) {
        ApplyCollisionResponse(obj1, physics1, obj2, physics2, mtv);
    }
}

// GJK collision detection
bool CollisionSystem_Yang::CheckCollision(const std::vector<glm::vec3> &shape1,
                                     const std::vector<glm::vec3> &shape2,
                                     std::vector<glm::vec3> &simplex) {
    glm::vec3 direction = glm::vec3(1, 0, 0);
    simplex.push_back(Support(shape1, direction) - Support(shape2, -direction));
    direction = -simplex.back();

    while (true) {
        glm::vec3 newPoint = Support(shape1, direction) - Support(shape2, -direction);
        if (glm::dot(newPoint, direction) < 0)
            return false;

        simplex.push_back(newPoint);

        if (simplex.size() == 4)
            return true; // Confirm collision when forming tetrahedron

        if (HandleSimplex(simplex, direction))
            return true;
    }
}

// Find the farthest point in a direction
glm::vec3 CollisionSystem_Yang::Support(const std::vector<glm::vec3> &shape, const glm::vec3 &direction) {
    float maxDot = -FLT_MAX;
    glm::vec3 bestPoint;

    for (const auto &point : shape) {
        float dotProduct = glm::dot(point, direction);
        if (dotProduct > maxDot) {
            maxDot = dotProduct;
            bestPoint = point;
        }
    }
    return bestPoint;
}

glm::vec3 CollisionSystem_Yang::ComputeMTV(std::vector<glm::vec3>& simplex,
                                      const std::vector<glm::vec3>& shape1,
                                      const std::vector<glm::vec3>& shape2) {
    struct Face {
        glm::vec3 a, b, c;
        glm::vec3 normal;
        float distance;
    };

    std::vector<glm::vec3> polytope = simplex;
    std::vector<Face> faces;
    glm::vec3 normal;

    for (size_t i = 0; i < simplex.size(); i++) {
        glm::vec3 A = simplex[i];
        glm::vec3 B = simplex[(i + 1) % simplex.size()];
        glm::vec3 C = simplex[(i + 2) % simplex.size()];

        glm::vec3 AB = B - A;
        glm::vec3 AC = C - A;
        glm::vec3 faceNormal = glm::normalize(glm::cross(AB, AC));

        if (glm::dot(faceNormal, A) < 0) faceNormal = -faceNormal;

        float distance = glm::dot(A, faceNormal);
        faces.push_back({A, B, C, faceNormal, distance});
    }

    int maxIterations = 100;
    int iteration = 0;

    while (iteration++ < maxIterations) {
        float minDistance = FLT_MAX;
        int closestFaceIdx = -1;

        for (size_t i = 0; i < faces.size(); i++) {
            if (faces[i].distance < minDistance) {
                minDistance = faces[i].distance;
                normal = faces[i].normal;
                closestFaceIdx = static_cast<int>(i);
            }
        }

        glm::vec3 supportPoint = Support(shape1, normal) - Support(shape2, -normal);
        float supportDistance = glm::dot(supportPoint, normal);

        // Stop if no significant expansion is possible
        if (supportDistance - minDistance < 0.001f) {
            float MTV_MIN = 0.005f;
            float MTV_MAX = 0.05f; // Limit large jumps
            float mtvLength = glm::clamp(supportDistance, MTV_MIN, MTV_MAX);
            return normal * mtvLength;
        }

        bool isDuplicate = false;
        for (const auto& point : polytope) {
            if (glm::length(point - supportPoint) < 1e-5f) {
                isDuplicate = true;
                break;
            }
        }
        if (isDuplicate) {
            return normal * std::max(minDistance, 0.005f);
        }

        polytope.push_back(supportPoint);
    }

    return glm::vec3(0, 0, 0); // 🚨 Last resort: return zero MTV if no correction found
}
void CollisionSystem_Yang::ApplyCollisionResponse(GameObject_Yang* obj1, PhysicsComponent* physics1,
                                             GameObject_Yang* obj2, PhysicsComponent* physics2,
                                             const glm::vec3& mtv) {
    if (glm::length(mtv) < 0.0001f) return;

    glm::vec3 mtvDirection = glm::normalize(mtv);
    bool isVerticalCollision = glm::abs(mtvDirection.y) > glm::abs(mtvDirection.x) &&
                               glm::abs(mtvDirection.y) > glm::abs(mtvDirection.z);

    float MTV_FIX_FACTOR = 1.02f; // Apply a slight extra correction

    if (isVerticalCollision) {
        if (!physics1->isStatic) {
            obj1->GetComponent<TransformComponent_Yang>()->transform.translate(glm::vec3(0, mtv.y * MTV_FIX_FACTOR, 0));
            physics1->velocity.y = 0;
            if (mtv.y > 0) {
                physics1->isGrounded = true;
            }
        }
        if (!physics2->isStatic) {
            obj2->GetComponent<TransformComponent_Yang>()->transform.translate(glm::vec3(0, -mtv.y * MTV_FIX_FACTOR, 0));
            physics2->velocity.y = 0;
            if (mtv.y < 0) {
                physics2->isGrounded = true;
            }
        }
    } else {
        if (!physics1->isStatic) {
            obj1->GetComponent<TransformComponent_Yang>()->transform.translate(mtv * 0.9f);
        }
        if (!physics2->isStatic) {
            obj2->GetComponent<TransformComponent_Yang>()->transform.translate(-mtv * 0.9f);
        }
    }
}

// 🔹 Handle Simplex Cases for GJK
bool CollisionSystem_Yang::HandleSimplex(std::vector<glm::vec3> &simplex, glm::vec3 &direction) {
    if (simplex.size() == 2) {
        glm::vec3 A = simplex[1];
        glm::vec3 B = simplex[0];
        glm::vec3 AB = B - A;
        glm::vec3 AO = -A;

        if (glm::dot(AB, AO) > 0) {
            direction = glm::cross(glm::cross(AB, AO), AB);
        } else {
            simplex = {A};
            direction = AO;
        }
        return false;
    }

    if (simplex.size() == 3) {
        glm::vec3 A = simplex[2];
        glm::vec3 B = simplex[1];
        glm::vec3 C = simplex[0];

        glm::vec3 AB = B - A;
        glm::vec3 AC = C - A;
        glm::vec3 AO = -A;
        glm::vec3 ABC = glm::cross(AB, AC);

        if (glm::dot(glm::cross(ABC, AC), AO) > 0) {
            simplex = {A, C};
            direction = glm::cross(glm::cross(AC, AO), AC);
            return false;
        }

        if (glm::dot(glm::cross(AB, ABC), AO) > 0) {
            simplex = {A, B};
            direction = glm::cross(glm::cross(AB, AO), AB);
            return false;
        }

        return true;
    }

    return false;
}
