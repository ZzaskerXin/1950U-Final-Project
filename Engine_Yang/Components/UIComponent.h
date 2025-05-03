#pragma once
#include "Component.h"
#include "Graphics/global.h"
#include <string>
#include <glm/glm.hpp>

class UIComponent : public Component {
public:
    std::string text;
    std::string font;
    glm::ivec2 position;
    AnchorPoint anchor;
    float scale;
    float depth;
    glm::vec3 color;

    UIComponent(const std::string& text,
                const std::string& font = "opensans",
                glm::ivec2 position = glm::ivec2(80, 260),
                AnchorPoint anchor = AnchorPoint::TopLeft,
                float scale = 1.0f,
                float depth = 0.1f,
                glm::vec3 color = glm::vec3(1, 0, 1))
        : text(text), font(font), position(position), anchor(anchor),
          scale(scale), depth(depth), color(color) {}
};
