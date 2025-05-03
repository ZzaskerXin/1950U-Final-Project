#Sets minimum cmake version 
cmake_minimum_required(VERSION 3.14)

#Sets project name to CS1950UEngine
project(CS1950UEngine)

find_package(OpenGL REQUIRED)

#Tells compiler to use c++ 20
set(CMAKE_CXX_STANDARD 20)
#Makes c++ 20 required
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Setting path macros
set(GLFW_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/External/glfw-3.3.8")
set(GLEW_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/External/glew")
set(GLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/External/glm-master")
set(STB_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/External/stb")
set(FREETYPE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/External/freetype")
set(IMGUI_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/External/imgui-docking")
set(RAUDIO_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/External/raudio")
set(SCREEN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Screen")
set(ECS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Engine_Yang")
set(ECS_COMPONENTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Engine_Yang/Components")
set(ECS_SYSTEMS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Engine_Yang/Systems")

#GLFW settings
set(BUILD_SHARED_LIBS OFF CACHE BOOL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")

#freetype settings
set(FT_DISABLE_ZLIB TRUE)
set(FT_DISABLE_BZIP2 TRUE)
set(FT_DISABLE_PNG TRUE)
set(FT_DISABLE_HARFBUZZ TRUE)
set(FT_DISABLE_BROTLI TRUE)


add_subdirectory(${GLFW_SOURCE_DIR})
add_subdirectory(${GLM_SOURCE_DIR})
add_subdirectory(${FREETYPE_SOURCE_DIR})
add_subdirectory(${IMGUI_SOURCE_DIR})
add_subdirectory(${RAUDIO_SOURCE_DIR})

add_library(StaticGLEW STATIC ${GLEW_SOURCE_DIR}/src/glew.c)

include_directories(${GLFW_SOURCE_DIR}/include
                    ${GLFW_SOURCE_DIR}/deps
                    ${GLEW_SOURCE_DIR}/include
                    ${STB_SOURCE_DIR}
                    ${FREETYPE_SOURCE_DIR}/include
                    ${PROJECT_SOURCE_DIR}
                    ${IMGUI_SOURCE_DIR}
                    ${RAUDIO_SOURCE_DIR}
                    ${SCREEN_DIR}
                    ${ECS_SYSTEMS_DIR}
                    ${ECS_DIR}
                    ${ECS_COMPONENTS_DIR}
                    )

add_executable(${PROJECT_NAME}
    main.cpp

    Engine/core.h
    Engine/core.cpp
    Engine/window.h
    Engine/window.cpp
    Engine/screen.h

    Game1/application.h
    Game1/gamescreen.h
    Game1/menuscreen.h
    Game1/application.cpp
    Game1/gamescreen.cpp
    Game1/menuscreen.cpp

    Graphics/graphics.h
    Graphics/graphics.cpp
    Graphics/global.h
    Graphics/debug.h
    Graphics/camera.h
    Graphics/camera.cpp
    Graphics/shaderloader.h
    Graphics/material.h
    Graphics/material.cpp
    Graphics/shapedata.h
    Graphics/shape.h
    Graphics/shape.cpp
    Graphics/modeltransform.h
    Graphics/modeltransform.cpp
    Graphics/light.h
    Graphics/light.cpp
    Graphics/font.h
    Graphics/font.cpp
    Graphics/textrenderer.h
    Graphics/textrenderer.cpp
    Graphics/tiny_obj_loader.h
    Graphics/GLWrappers/shader.h
    Graphics/GLWrappers/shader.cpp
    Graphics/GLWrappers/texture.h
    Graphics/GLWrappers/texture.cpp
    Graphics/GLWrappers/vbo.h
    Graphics/GLWrappers/vbo.cpp
    Graphics/GLWrappers/vao.h
    Graphics/GLWrappers/vao.cpp

    # Shapes
    Graphics/shapes/Cone.h
    Graphics/shapes/Cone.cpp
    Graphics/shapes/Cube.h
    Graphics/shapes/Cube.cpp
    Graphics/shapes/Cylinder.h
    Graphics/shapes/Cylinder.cpp
    Graphics/shapes/Sphere.h
    Graphics/shapes/Sphere.cpp
    Graphics/shapes/mesh.h
    Graphics/shapes/mesh.cpp
    Graphics/shapes/Quad.cpp
    Graphics/shapes/Quad.h
    Engine/component.h


    Engine/system.h
    Engine/gameobject.h
    Engine/rendercomponent.h
    Engine/transformcomponent.h
    Game1/drawsystem.h
    Game1/controlsystem.h
    Engine/collisioncomponent.h
    Game1/collisionsystem.h
    Engine/UniformGrid.h
    Game1/aisystem.h

    Screen/Screen.h
    Screen/ScreenManager.h
    Screen/ScreenManager.cpp

    Engine_Yang/Component.h
    Engine_Yang/GameObject.h
    Engine_Yang/GameWorld.h
    Engine_Yang/GameWorld.cpp
    Engine_Yang/InputMap.h
    Engine_Yang/System.h

    Engine_Yang/Systems/CharacterControllerSystem.cpp
    Engine_Yang/Systems/CharacterControllerSystem.h
    Engine_Yang/Systems/DrawSystem.h
    Engine_Yang/Systems/DrawSystem.cpp
    Engine_Yang/Systems/CollisionSystem.cpp
    Engine_Yang/Systems/CollisionSystem.h

    Engine_Yang/Components/MeshComponent.h
    Engine_Yang/Components/PhysicsComponent.h
    Engine_Yang/Components/TransformComponent.h

    Game_Parkour/GameScreen.h
    Game_Parkour/GameScreen.cpp
    Game_Parkour/MenuScreen.cpp
    Game_Parkour/MenuScreen.h
    Game_Parkour/Game.h
    Game_Parkour/Game.cpp
    Game_Parkour/WinScreen.cpp
    Game_Parkour/WinScreen.h
    Game_Parkour/LossScreen.cpp
    Game_Parkour/LossScreen.h


















)

target_link_libraries(${PROJECT_NAME} glfw StaticGLEW glm freetype imgui raudio ${OPENGL_LIBRARIES})

file( COPY ${CMAKE_CURRENT_SOURCE_DIR}/Resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )

if (WIN32)
  add_compile_definitions(GLEW_STATIC)
  target_link_libraries(${PROJECT_NAME}
      opengl32
      glu32
  )
endif()

if (UNIX AND NOT APPLE)
  target_link_libraries(${PROJECT_NAME}
      GL
  )
endif()
