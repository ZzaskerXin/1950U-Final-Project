#include "GameWorld.h"
#include "DrawSystem_Yang.h"

void GameWorld::AddGameObject(std::shared_ptr<GameObject_Yang> obj) {
    objects.push_back(obj);
}

void GameWorld::AddSystem(std::shared_ptr<System_Yang> system) {
    std::cout << "add system for gameworld" << std::endl;
    
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
        if (auto drawSystem = std::dynamic_pointer_cast<DrawSystem_Yang>(system)) {
            drawSystem->Render();
        }
    }
}

std::vector<std::shared_ptr<GameObject_Yang>>& GameWorld::GetObjects() {
    return objects;
}
