#pragma once
#include "../System_Yang.h"
#include "../GameObject_Yang.h"
#include "../Components/TransformComponent_Yang.h"
#include "../Components/PhysicsComponent.h"
#include "MeshComponent.h"

class CharacterControllerSystem : public System_Yang {
public:
    void Update(float deltaTime) override;
};
