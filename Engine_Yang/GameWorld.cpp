#include "GameWorld.h"
#include "DrawSystem.h"

void GameWorld::AddGameObject(std::shared_ptr<GameObject> obj) {
    objects.push_back(obj);
}

void GameWorld::AddSystem(std::shared_ptr<System> system) {
    systems.push_back(system);
    system->SetGameWorld(this->shared_from_this());
}

void GameWorld::Update(float deltaTime) {


    for (auto& system : systems) {
        system->Update(deltaTime);
    }
    
}

void GameWorld::Draw() {
    for (auto& system : systems) {
        if (auto drawSystem = std::dynamic_pointer_cast<DrawSystem>(system)) {
            drawSystem->Render();
        }
    }
}

std::vector<std::shared_ptr<GameObject>>& GameWorld::GetObjects() {
    return objects;
}
