#pragma once
#include "System.h"
#include "GameObject.h"
#include <memory>
#include "MeshComponent.h"
#include "TransformComponent.h"
#include <GLFW/glfw3.h>
#include "GameWorld.h"


class DrawSystem : public System {
private:
    std::string shaderName;
public:
    DrawSystem(const std::string& shader) : shaderName(shader) {} 

    void Initialize();  
    void Render();
    void Update(float deltaTime) override {}  
};
