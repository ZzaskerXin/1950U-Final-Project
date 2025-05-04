#pragma once
#include "System_Yang.h"
#include "GameObject_Yang.h"
#include <memory>
#include "MeshComponent.h"
#include "TransformComponent_Yang.h"
#include <GLFW/glfw3.h>
#include "GameWorld.h"


class DrawSystem_Yang : public System_Yang {
private:
    std::string shaderName;
public:
DrawSystem_Yang(const std::string& shader) : shaderName(shader) {} 

    void Initialize();  
    void Render();
    void Update(float deltaTime) override {}  
};
