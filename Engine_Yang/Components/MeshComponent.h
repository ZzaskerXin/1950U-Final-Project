#pragma once
#include "Component.h"
#include <string>
#include <memory>
#include "Graphics/global.h"

class MeshComponent : public Component {
public:
    std::string modelType;
    std::string shapeName;
    std::shared_ptr<Material> material;
    bool display = true;

    MeshComponent(const std::string& type, const std::string& shape, const std::string& texture = "")
        : modelType(type), shapeName(shape) {
        // std::cout << "init mesh" << std::endl;

        if (!texture.empty()) {
            // if (!material) {

            material = Global::graphics.addMaterial(texture, "Resources/Images/" + texture, 0.5);
            // }
        }
    }
};
