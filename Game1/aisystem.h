#include "Engine/gameobject.h"
#include "Engine/system.h"
#include "Engine/aicomponent.h"

class AISystem : public System {
public:
    std::vector<std::shared_ptr<gameobject>> agents;

    void update(float deltaTime) {
        float minDistance = 1.5f;
        float fovAngle = 120.0f;
        float chaseStopTime = 3.0f;

        for (auto& obj : agents) {
            auto ai = obj->getComponent<AIComponent>();
            auto trans = obj->getComponent<TransformComponent>();
            auto targetTrans = ai->target->getComponent<TransformComponent>();

            glm::vec3 myPos = trans->transform->getPos();
            glm::vec3 playerPos = targetTrans->transform->getPos();
            glm::vec3 toPlayer = playerPos - myPos;
            float distToPlayer = glm::length(toPlayer);

            glm::vec3 forward = glm::normalize(ai->velocity);
            if (glm::length(forward) < 0.01f) forward = glm::vec3(0, 0, 1);
            float dotVal = glm::dot(glm::normalize(toPlayer), forward);
            bool canSeePlayer = dotVal > glm::cos(glm::radians(fovAngle / 2.0f));

            switch (ai->state) {
            case AIState::Patrol:
                if (canSeePlayer) {
                    ai->state = AIState::Chase;
                    ai->timeSinceLastSeen = 0.0f;
                    std::cout << "[AI] Switched to CHASE\n";
                } else {
                    glm::vec3 patrolTarget = ai->patrolPoints[ai->currentPatrolIndex];
                    glm::vec3 dir = glm::normalize(patrolTarget - myPos);
                    ai->velocity = dir * ai->speed;

                    if (glm::length(patrolTarget - myPos) < 0.5f) {
                        ai->currentPatrolIndex = (ai->currentPatrolIndex + 1) % ai->patrolPoints.size();
                    }

                    std::cout << "[AI] Patrolling to point "
                              << ai->currentPatrolIndex << " at "
                              << patrolTarget.x << ", " << patrolTarget.z << "\n";
                }
                break;

            case AIState::Chase:
                if (canSeePlayer) {
                    ai->timeSinceLastSeen = 0.0f;

                    if (distToPlayer > minDistance) {
                        glm::vec3 dir = glm::normalize(toPlayer);
                        ai->velocity = dir * ai->speed;
                    } else {
                        ai->velocity = glm::vec3(0.0f);
                    }

                    std::cout << "[AI] Chasing player. Velocity: "
                              << ai->velocity.x << ", " << ai->velocity.z << "\n";
                } else {
                    ai->timeSinceLastSeen += deltaTime;
                    if (ai->timeSinceLastSeen > chaseStopTime) {
                        ai->state = AIState::Patrol;
                        ai->velocity = glm::vec3(0.0f);
                        std::cout << "[AI] Lost player. Returning to PATROL\n";
                    }
                }
                break;
            }
        }
    }

    void addAgent(std::shared_ptr<gameobject> obj) {
        agents.push_back(obj);
    }
};
