#pragma once
#include "../System.h"
#include "../GameObject.h"
#include "../Components/TransformComponent.h"
#include "../Components/PhysicsComponent.h"
#include "MeshComponent.h"

class CharacterControllerSystem : public System {
public:
    void Update(float deltaTime) override;
};
