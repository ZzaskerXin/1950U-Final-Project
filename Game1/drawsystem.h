#include "Engine/gameobject.h"
#include "Engine/system.h"

class DrawSystem : public System{
public:

    void addGameObject(std::shared_ptr<gameobject> object) {
        gameObjects.push_back(object);
    }

    void setCamera(std::shared_ptr<Camera> cam) {
        camera = cam;
    }

    void render() {
        Global::graphics.setClearColor(glm::vec4(0.0f, 0.5f, 1.0f, 1.0f));
        Global::graphics.clearScreen(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Global::graphics.bindShader("phong");
        Global::graphics.setGlobalData(glm::vec3(0.5f));
        Global::graphics.setCameraData(camera);

        for (const auto& object : gameObjects) {
            Global::graphics.drawShape(object->getComponent<RenderComponent>()->shape, object->getComponent<TransformComponent>()->transform,
                                       Global::graphics.getMaterial(object->getComponent<RenderComponent>()->material));
        }
    }

private:
    std::shared_ptr<Camera> camera;
    std::vector<std::shared_ptr<gameobject>> gameObjects;
};
