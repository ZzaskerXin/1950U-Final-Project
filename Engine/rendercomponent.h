#pragma once

#include "Graphics/global.h"
#include <GLFW/glfw3.h>
#include "component.h"

class RenderComponent : public Component
{
public:
    std::shared_ptr<Shape> shape = nullptr;
    std::string material;
};
