#pragma once
#include <unordered_map>

#include <GLFW/glfw3.h>

class InputMap {
private:
    std::unordered_map<int, bool> keyStates;

public:
    void SetKeyState(int key, bool isPressed) {
        keyStates[key] = isPressed;
    }

    bool IsKeyPressed(int key) const {
        auto it = keyStates.find(key);
        return (it != keyStates.end()) ? it->second : false;
    }
};
