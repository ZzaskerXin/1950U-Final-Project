#pragma once

#include "Graphics/global.h"
#include <GLFW/glfw3.h>

class Component {
public:
    virtual ~Component() = default;
    Component() = default;
};
