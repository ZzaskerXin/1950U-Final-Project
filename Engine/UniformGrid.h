#pragma once

#include "Graphics/global.h"
#include <GLFW/glfw3.h>

struct GridCell {
    std::vector<int> objectIndices;
};

struct IVec3Hasher {
    size_t operator()(const glm::ivec3& v) const {
        size_t hx = std::hash<int>()(v.x);
        size_t hy = std::hash<int>()(v.y);
        size_t hz = std::hash<int>()(v.z);
        return ((hx ^ (hy << 1)) >> 1) ^ (hz << 1);
    }
};

class UniformGrid {
public:
    float cellSize = 1.5f;
    std::unordered_map<glm::ivec3, GridCell, IVec3Hasher> grid;

    glm::ivec3 worldToCell(const glm::vec3& pos) const {
        return glm::ivec3(floor(pos.x / cellSize), floor(pos.y / cellSize), floor(pos.z / cellSize));
    }

    void clear() {
        grid.clear();
    }

    void insertObject(const glm::vec3& pos, int index) {
        glm::ivec3 cell = worldToCell(pos);
        grid[cell].objectIndices.push_back(index);
    }

    std::vector<int> getNearbyIndices(const glm::vec3& pos) const {
        glm::ivec3 base = worldToCell(pos);
        std::vector<int> result;
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                for (int dz = -1; dz <= 1; dz++) {
                    glm::ivec3 neighbor = base + glm::ivec3(dx, dy, dz);
                    auto it = grid.find(neighbor);
                    if (it != grid.end()) {
                        result.insert(result.end(), it->second.objectIndices.begin(), it->second.objectIndices.end());
                    }
                }
            }
        }
        return result;
    }
};
