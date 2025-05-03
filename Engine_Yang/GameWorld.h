#pragma once
#include <vector>
#include <memory>
#include "DrawSystem.h"
#include "InputMap.h"
#include "System.h"


class GameObject;

class GameWorld : public std::enable_shared_from_this<GameWorld> {
private:
    std::vector<std::shared_ptr<GameObject>> objects;
    std::vector<std::shared_ptr<System>> systems;
    InputMap inputMap;

public:
    void AddGameObject(std::shared_ptr<GameObject> obj);

    void AddSystem(std::shared_ptr<System> system) ;
    void Update(float deltaTime) ;

    void Draw() ;

    std::vector<std::shared_ptr<GameObject>>& GetObjects() ;

    InputMap& GetInputMap() { return inputMap; } 

    template <typename T>
    std::shared_ptr<T> GetSystem() {
        for (auto& system : systems) {
            if (auto specificSystem = std::dynamic_pointer_cast<T>(system)) {
                return specificSystem;
            }
        }
        return nullptr; // Return null if the system isn't found
    }

    std::shared_ptr<GameObject> GetObjectById(int id) {
        for (auto& obj : objects) {
            if (obj->GetId() == id) {
                return obj;
            }
        }
        return nullptr;
    }
    

};
