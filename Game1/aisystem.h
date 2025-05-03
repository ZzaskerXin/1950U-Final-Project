#include "Engine/gameobject.h"
#include "Engine/system.h"
#include "glm/gtx/string_cast.hpp"

struct Agent {
    glm::vec3 target;
    std::vector<glm::vec3> path;
    int pathIndex = 0;
    float speed = 0.0f;
};

class AISystem : public System{
public:

    std::vector<glm::vec3> findPath(const glm::vec3& start, const glm::vec3& goal) {
        std::vector<glm::vec3> path;
        path.push_back(start);
        path.push_back(goal);
        return path;
    }

    void addAgent(int id, const glm::vec3& start, const glm::vec3& goal) {
        Agent& agent = agents[id];
        agent.target = goal;
        agent.path = findPath(start, goal);
        agent.pathIndex = 0;
        if (agent.speed <= 0.0f)
            agent.speed = 1.5f + static_cast<float>(rand()) / RAND_MAX * 2.0f;
    }

    void updateAgentDirectionsAndSpeeds(std::vector<glm::vec3>& directions,
                                        std::vector<float>& speeds,
                                        const std::vector<std::shared_ptr<gameobject>>& objects,
                                        float deltaTime) {
        for (size_t i = 0; i < objects.size(); ++i) {
            if (!agents.count(i)) continue;

            Agent& agent = agents[i];
            glm::vec3 pos = objects[i]->getComponent<TransformComponent>()->transform->getPos();

            while (agent.pathIndex < agent.path.size() &&
                   glm::length(agent.path[agent.pathIndex] - pos) < 0.0025f) {
                agent.pathIndex++;
            }

            if (agent.pathIndex >= agent.path.size()) continue;

            glm::vec3 target = agent.path[agent.pathIndex];
            glm::vec3 dir = target - pos;

            if (glm::length(dir) > 0.0001f) {
                dir = glm::normalize(dir);
                if (i < directions.size()) directions[i] = dir;
            }

            if (i < speeds.size() && speeds[i] <= 0.0f)
                speeds[i] = agent.speed;

            std::cout << "[AI] Agent " << i << " pos = " << glm::to_string(pos)
                      << " target = " << glm::to_string(agent.target)
                      << " idx = " << agent.pathIndex << "/" << agent.path.size() << std::endl;
        }
    }

    void clearAgents() {
        agents.clear();
    }

private:
    std::unordered_map<int, Agent> agents;
};
