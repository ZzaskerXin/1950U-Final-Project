#pragma once

#include "Graphics/global.h"
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <typeindex>
#include "rendercomponent.h"
#include "transformcomponent.h"
#include "collisioncomponent.h"

class gameobject
{
public:
    template <class T>
    void addComponent() {
        m_components[std::type_index(typeid(T))] = std::make_shared<T>();
    }

    template <class T>
    void removeComponent() {
        m_components.erase(std::type_index(typeid(T)));
    }

    template <class T>
    std::shared_ptr<T> getComponent() {
        auto it = m_components.find(std::type_index(typeid(T)));
        if (it != m_components.end()) {
            return std::static_pointer_cast<T>(it->second);
        }
        return nullptr;
    }
private:
    std::unordered_map<std::type_index, std::shared_ptr<Component>> m_components;
};

