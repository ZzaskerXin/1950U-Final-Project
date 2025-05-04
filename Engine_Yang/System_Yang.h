#pragma once
#include <vector>
#include <memory>
#include <algorithm>
// #include "GameWorld.h"

class GameObject_Yang;
class GameWorld;

class System_Yang {
protected:
    std::vector<std::shared_ptr<GameObject_Yang>> objects;
    std::shared_ptr<GameWorld> gameWorld = nullptr;

public:
    virtual ~System_Yang() = default;
    
    void AddObject(std::shared_ptr<GameObject_Yang> obj) {
        objects.push_back(obj);
    }

    void RemoveObject(std::shared_ptr<GameObject_Yang> obj) {
        objects.erase(std::remove(objects.begin(), objects.end(), obj), objects.end());
    }
    void SetGameWorld(std::shared_ptr<GameWorld> world) { gameWorld = world; } 

    virtual void Update(float deltaTime) = 0;
};
