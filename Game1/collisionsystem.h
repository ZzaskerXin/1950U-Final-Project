#include "Engine/gameobject.h"
#include "Engine/system.h"
#include "glm/gtx/component_wise.hpp"
#include <glm/gtx/string_cast.hpp>

struct Triangle {
    glm::vec3 v0, v1, v2;
};

struct AABB {
    glm::vec3 min, max;

    bool intersects(const glm::vec3& point, float radius) const {
        glm::vec3 closest = glm::clamp(point, min, max);
        return glm::distance(point, closest) <= radius;
    }
};

struct BVHNode {
    AABB box;
    std::vector<Triangle> triangles;
    std::unique_ptr<BVHNode> left;
    std::unique_ptr<BVHNode> right;
};

class CollisionSystem : public System{
public:
    std::unique_ptr<BVHNode> buildBVH(std::vector<Triangle>& tris, int depth = 0) {
        if (tris.empty()) return nullptr;

        bvhNodeCount++;
        bvhMaxDepth = std::max(bvhMaxDepth, depth);

        auto node = std::make_unique<BVHNode>();
        node->triangles = tris;

        glm::vec3 min(FLT_MAX), max(-FLT_MAX);
        for (const auto& tri : tris) {
            for (const auto& v : {tri.v0, tri.v1, tri.v2}) {
                min = glm::min(min, v);
                max = glm::max(max, v);
            }
        }
        node->box = {min, max};

        if (tris.size() <= 4 || depth > 16) return node;

        glm::vec3 size = max - min;
        int axis = (size.x > size.y && size.x > size.z) ? 0 : (size.y > size.z ? 1 : 2);

        std::sort(tris.begin(), tris.end(), [axis](const Triangle& a, const Triangle& b) {
            float ca = (a.v0[axis] + a.v1[axis] + a.v2[axis]) / 3.0f;
            float cb = (b.v0[axis] + b.v1[axis] + b.v2[axis]) / 3.0f;
            return ca < cb;
        });

        size_t mid = tris.size() / 2;
        std::vector<Triangle> leftTris(tris.begin(), tris.begin() + mid);
        std::vector<Triangle> rightTris(tris.begin() + mid, tris.end());

        node->left = buildBVH(leftTris, depth + 1);
        node->right = buildBVH(rightTris, depth + 1);
        node->triangles.clear();

        return node;
    }

    void queryBVH(BVHNode* node, const glm::vec3& pos, float radius, std::vector<Triangle>& outTris) const {
        if (!node || !node->box.intersects(pos, radius)) return;

        if (!node->left && !node->right) {
            outTris.insert(outTris.end(), node->triangles.begin(), node->triangles.end());
        } else {
            queryBVH(node->left.get(), pos, radius, outTris);
            queryBVH(node->right.get(), pos, radius, outTris);
        }
    }

    void setTerrain(const std::vector<float>& terrainData) {
        // terrainVertices = terrainData;
        terrainVertices = terrainData;
        terrainTriangles.clear();
        for (size_t i = 0; i < terrainData.size(); i += 9) {
            terrainTriangles.push_back({
                {terrainData[i], terrainData[i+1], terrainData[i+2]},
                {terrainData[i+3], terrainData[i+4], terrainData[i+5]},
                {terrainData[i+6], terrainData[i+7], terrainData[i+8]}
            });
        }

        bvhNodeCount = 0;
        bvhMaxDepth = 0;
        bvhRoot = buildBVH(terrainTriangles);

        // std::cout << "[BVH] Built BVH with " << bvhNodeCount << " nodes, "
        //           << "max depth = " << bvhMaxDepth << ", "
        //           << "triangles = " << terrainTriangles.size() << std::endl;
    }

    std::vector<float> getTerrain() const {
        return terrainVertices;
    }

    glm::vec3 closestPointOnSegment(const glm::vec3& a, const glm::vec3& b, const glm::vec3& point) {
        glm::vec3 ab = b - a;
        float t = glm::dot(point - a, ab) / glm::dot(ab, ab);
        t = glm::clamp(t, 0.0f, 1.0f);
        return a + t * ab;
    }

    glm::vec3 closestPointOnTriangle(const glm::vec3& point, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) {
        glm::vec3 edge0 = v1 - v0;
        glm::vec3 edge1 = v2 - v0;
        glm::vec3 v0_to_point = point - v0;

        float dot00 = glm::dot(edge0, edge0);
        float dot01 = glm::dot(edge0, edge1);
        float dot02 = glm::dot(edge0, v0_to_point);
        float dot11 = glm::dot(edge1, edge1);
        float dot12 = glm::dot(edge1, v0_to_point);

        float denom = dot00 * dot11 - dot01 * dot01;

        if (denom == 0.0f) {
            return v0;
        }

        float u = (dot11 * dot02 - dot01 * dot12) / denom;
        float v = (dot00 * dot12 - dot01 * dot02) / denom;

        if (u >= 0 && v >= 0 && (u + v) <= 1) {
            return v0 + u * edge0 + v * edge1;
        }

        glm::vec3 closest = v0;
        float minDist = glm::distance(point, v0);

        glm::vec3 candidates[] = {
            closestPointOnSegment(v0, v1, point),
            closestPointOnSegment(v1, v2, point),
            closestPointOnSegment(v2, v0, point)
        };

        for (const auto& candidate : candidates) {
            float dist = glm::distance(point, candidate);
            if (dist < minDist) {
                minDist = dist;
                closest = candidate;
            }
        }

        return closest;
    }

    glm::vec3 ellipsoidCollisionResponse(const glm::vec3& objectPos, const glm::vec3& objectRadius/*, const std::vector<float>& vertices*/) {
        std::vector<Triangle> nearby;
        queryBVH(bvhRoot.get(), objectPos, glm::compMax(objectRadius), nearby);

        // std::cout << "[BVH] Query at " << glm::to_string(objectPos)
        //           << " returned " << nearby.size() << " triangle(s)" << std::endl;

        glm::vec3 closestPoint;
        float minDist = FLT_MAX;

        for (const auto& tri : nearby) {
            glm::vec3 pt = closestPointOnTriangle(objectPos, tri.v0, tri.v1, tri.v2);
            float dist = glm::distance(objectPos, pt);
            if (dist < minDist) {
                minDist = dist;
                closestPoint = pt;
            }
        }
        return closestPoint + objectRadius - objectPos;
    }


    glm::vec3 closestPointOnTerrain(const glm::vec3& position) {
        glm::vec3 closestPoint;
        float minDist = std::numeric_limits<float>::max();

        for (size_t i = 0; i < terrainVertices.size(); i += 9) {
            glm::vec3 v0(terrainVertices[i], terrainVertices[i + 1], terrainVertices[i + 2]);
            glm::vec3 v1(terrainVertices[i + 3], terrainVertices[i + 4], terrainVertices[i + 5]);
            glm::vec3 v2(terrainVertices[i + 6], terrainVertices[i + 7], terrainVertices[i + 8]);

            glm::vec3 point = closestPointOnTriangle(position, v0, v1, v2);
            float dist = glm::distance(position, point);

            if (dist < minDist) {
                minDist = dist;
                closestPoint = point;
            }
        }
        return closestPoint;
    }

    bool isCollidingWithGround(const glm::vec3& position, float radius) {
        glm::vec3 closestPoint = closestPointOnTerrain(position);
        return (position.y - radius) <= closestPoint.y;
    }
private:
    std::vector<float> terrainVertices;
    std::unique_ptr<BVHNode> bvhRoot;
    std::vector<Triangle> terrainTriangles;
    int bvhNodeCount = 0;
    int bvhMaxDepth = 0;
};
