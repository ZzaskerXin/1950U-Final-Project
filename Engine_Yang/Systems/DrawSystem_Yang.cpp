#include "DrawSystem_Yang.h"
#include "Graphics/global.h"


#include "DrawSystem_Yang.h"
#include "Graphics/global.h"

#include "DrawSystem_Yang.h"
#include "Graphics/global.h"

#include "CameraSystem.h"
#include "UIComponent.h"

void DrawSystem_Yang::Render() {
    if(shaderName=="text") {
        Global::graphics.setClearColor(glm::vec3(0.1f, 0.1f, 0.1f)); 
        Global::graphics.clearScreen(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        std::shared_ptr<Camera> cam = gameWorld->GetSystem<CameraSystem>()->GetCamera();
        Global::graphics.setCameraData(cam);

        Global::graphics.bindShader("text");
        Global::graphics.setGlobalData(glm::vec3(0.5f));
        Global::graphics.setCameraData(cam);


        for (auto& obj : gameWorld->GetObjects()) {
            auto* ui = obj->GetComponent<UIComponent>();
            if (ui) {
                Global::graphics.drawUIText(Global::graphics.getFont(ui->font), ui->text, 
                    ui->position, ui->anchor, Global::graphics.getFramebufferSize().x, 
                    ui->scale, ui->depth, ui->color);
            }
        }


    }
    if(shaderName == "phong") {

        Global::graphics.setGlobalData(glm::vec3(0.5f));
        std::shared_ptr<Camera> cam = gameWorld->GetSystem<CameraSystem>()->GetCamera();
        Global::graphics.setCameraData(cam);
    
    
        Global::graphics.setClearColor(glm::vec3(1));
        Global::graphics.clearScreen(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Global::graphics.bindShader(shaderName);

        for (auto& obj : gameWorld->GetObjects()) {
            auto* transform = obj->GetComponent<TransformComponent_Yang>();
            auto* mesh = obj->GetComponent<MeshComponent>();

            if (!transform || !mesh ) continue;
            if(!mesh->display) continue;
            
            std::shared_ptr<Shape> model;
            if(mesh->modelType == "primitive") {
                model = Global::graphics.getShape(mesh->shapeName);

                if (model) {
                    Global::graphics.drawShape(model, transform->transform.getModelMatrix(), mesh->material);
                }
                // std::cout<< "drawing fucking prim shape" << std::endl;

            } 
            else if (mesh->modelType == "custom") {

                
                // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // wireframe mode for debug
                Global::graphics.drawObj(mesh->shapeName, transform->transform.getModelMatrix());
                // std::cout<< "drawing custom shape with name:" << mesh->shapeName << std::endl;
            }
        }
    }
}
