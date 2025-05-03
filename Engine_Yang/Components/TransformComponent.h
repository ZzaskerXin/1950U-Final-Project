#pragma once
#include "Component.h"
#include "Graphics/global.h"

class TransformComponent : public Component {
public:
    ModelTransform transform;

    TransformComponent() = default;
};
