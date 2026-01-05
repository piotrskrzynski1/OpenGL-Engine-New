#include "Cube/Cube.hpp"
#include "Shader/Shader.hpp"
#include "Texture/Texture.hpp"
#include "Engine/GameObjectComponents/MeshRenderer.hpp" // Ensure this file is in your include path

#include <Engine/Managers/ServiceLocator.hpp>
#include "Engine/RenderContext.hpp"
#include "GameObjects/Camera/Camera.hpp"

#include <OPENGL/glad/glad.h>
#include <GLFW/glfw3.h>
#include <OPENGL/glm/glm.hpp>
#include <OPENGL/glm/gtc/matrix_transform.hpp>
#include <iostream>

Cube::Cube(glm::vec3 pos) {
    position = pos;
    
    // 1. Initialize the MeshRenderer with the path
    // This handles Assimp loading, VAO/VBO generation internally
    meshRenderer = std::make_unique<MeshRenderer>("C:/Users/piotr/Downloads/DamagedHelmet.glb");
    
    // Manually set owner if your MeshRenderer relies on 'owner->position' 
    // (If you haven't fully implemented the generic Component system yet, we do this manually)
    meshRenderer->SetOwner(this); 

    // 2. Setup Shader & Texture
    shader = std::make_unique<Shader>("Shaders/TestShaders/Phong.vert", "Shaders/TestShaders/Phong.frag");
    texture = std::make_unique<Texture>("Textures/temp/texture.png");

    shader->use();
    shader->setInt("texture1", 0);
    shader->setVec4("ambientColor", glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
}

Cube::~Cube() {
    // Unique_ptr handles deletion of meshRenderer and shader automatically
}

void Cube::Update(double deltaTime) {
    texture->bind(0);
    shader->use();

    // --- SETUP GLOBAL UNIFORMS (View/Proj/Lights) ---
    auto renderService = ServiceLocator::Get().GetService<RenderContext>();
    
    shader->setMat4("view", renderService->GetViewMatrix());
    shader->setMat4("projection", renderService->GetProjectionMatrix());

    auto cam = renderService->GetMainCamera();
    if (cam) shader->setVec3("viewPos", cam->GetPosition());

    auto& lightManager = renderService->GetLightManager();
    lightManager.UpdateShader(*shader);

    // --- DELEGATE DRAWING TO THE COMPONENT ---
    // The MeshRenderer calculates the Model matrix (using this->position) and draws meshes
    meshRenderer->Draw(*shader);
}