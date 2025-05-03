#pragma once

#include "Graphics/global.h"
#include <GLFW/glfw3.h>

#include "component.h"

class TransformComponent : public Component
{
public:
    std::shared_ptr<ModelTransform> transform = nullptr;
};
