#pragma once

#include "Graphics/global.h"
#include <GLFW/glfw3.h>
#include "component.h"
#include "gameobject.h"

enum class AIState { Patrol, Chase };

class AIComponent : public Component {
public:
    std::shared_ptr<gameobject> target;
    float speed = 2.0f;
    glm::vec3 velocity = glm::vec3(0.0f);
    AIState state = AIState::Patrol;

    std::vector<glm::vec3> patrolPoints;
    int currentPatrolIndex = 0;

    float timeSinceLastSeen = 0.0f;
};
