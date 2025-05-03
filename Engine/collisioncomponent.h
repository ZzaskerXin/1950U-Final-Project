#pragma once

#include "Graphics/global.h"
#include <GLFW/glfw3.h>
#include "component.h"

class CollisionComponent : public Component
{
public:
    glm::vec3 radius;
};
