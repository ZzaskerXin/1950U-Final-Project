#pragma once
#include <unordered_map>
#include <typeindex>
#include <memory>

class Component;

class GameObject {
private:
    std::unordered_map<std::type_index, std::shared_ptr<Component>> components;

public:
    int id;
    
    explicit GameObject(int id) : id(id) {}

    template <typename T, typename... Args>
    std::shared_ptr<T> AddComponent(Args&&... args) {
        auto component = std::make_shared<T>(std::forward<Args>(args)...);
        components[std::type_index(typeid(T))] = component;
        return component;  
    }
    template <typename T>
    T* GetComponent() {
        auto it = components.find(std::type_index(typeid(T)));
        return (it != components.end()) ? static_cast<T*>(it->second.get()) : nullptr;
    }

    template <typename T>
    void RemoveComponent() {
        components.erase(std::type_index(typeid(T)));
    }

    int GetId() const { return id; }

};
