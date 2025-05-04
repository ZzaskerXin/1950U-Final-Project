#pragma once
#include "Component_Yang.h"
#include "Graphics/global.h"

class TransformComponent_Yang : public Component_Yang {
public:
    ModelTransform transform;

    TransformComponent_Yang() = default;
};
