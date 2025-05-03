#pragma once
#include <vector>
#include <memory>
#include <algorithm>
// #include "GameWorld.h"

class GameObject;
class GameWorld;

class System {
protected:
    std::vector<std::shared_ptr<GameObject>> objects;
    std::shared_ptr<GameWorld> gameWorld = nullptr;

public:
    virtual ~System() = default;
    
    void AddObject(std::shared_ptr<GameObject> obj) {
        objects.push_back(obj);
    }

    void RemoveObject(std::shared_ptr<GameObject> obj) {
        objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end());
    }
    void SetGameWorld(std::shared_ptr<GameWorld> world) { gameWorld = world; } 

    virtual void Update(float deltaTime) = 0;
};
