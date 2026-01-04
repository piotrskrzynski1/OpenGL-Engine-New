#include "Cube/Cube.hpp"
#include "Shader/Shader.hpp"
#include "Texture/Texture.hpp"

// --- ENGINE INCLUDES ---
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
    std::cout << "[Cube] Initializing at ("
        << pos.x << ", " << pos.y << ", " << pos.z << ")\n";

    InitMesh();

    // distinct shader/texture for THIS instance
    shader = std::make_unique<Shader>("Shaders/TestShaders/Phong.vert",
        "Shaders/TestShaders/Phong.frag");
    texture = std::make_unique<Texture>("Textures/temp/texture.png");

    shader->use();
    shader->setInt("texture1", 0);
    shader->setVec4("ambientColor",glm::vec4(0.1f,0.1f,0.1f,1.0f));
}

Cube::~Cube() {
    std::cout << "[Cube] Destroying at ("
        << position.x << ", " << position.y << ", " << position.z << ")\n";
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

// FIXED: Changed 'float' to 'double' to match Cube.hpp and GameObject.hpp
void Cube::Update(double deltaTime) {
    texture->bind(0);
    shader->use();

    // Model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

    // --- SERVICE LOCATOR ACCESS ---
    auto renderService = ServiceLocator::Get().GetService<RenderContext>();

    // Get shared view/projection
    const glm::mat4& view = renderService->GetViewMatrix();
    const glm::mat4& projection = renderService->GetProjectionMatrix();

    // Set Uniforms
    shader->setMat4("model", model);
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    shader->setMat3("normalMatrix", normalMatrix);

    // Get Camera Position
    auto cam = renderService->GetMainCamera();
    if (cam) {
        shader->setVec3("viewPos", cam->GetPosition());
    }

    // Get Lights
    auto& lightManager = renderService->GetLightManager();
    lightManager.UpdateShader(*shader); 

    // Draw
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void Cube::InitMesh() {
    // Format: X, Y, Z,    NX, NY, NZ,    U, V
    float vertices[] = {
        // FRONT FACE (Normal: 0, 0, 1)
        -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f,

        // BACK FACE (Normal: 0, 0, -1)
         0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f,

         // LEFT FACE (Normal: -1, 0, 0)
         -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
         -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
         -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
         -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,

         // RIGHT FACE (Normal: 1, 0, 0)
          0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
          0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
          0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
          0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,

          // TOP FACE (Normal: 0, 1, 0)
          -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
           0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 1.0f,
           0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
          -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 0.0f,

          // BOTTOM FACE (Normal: 0, -1, 0)
          -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
           0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
           0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 1.0f,
          -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f
    };

    unsigned int indices[] = {
        0, 1, 2,  2, 3, 0,       // FRONT
        5, 6, 4,  7, 4, 6,       // BACK
        8, 9, 10, 10, 11, 8,     // LEFT
        14, 13, 12, 12, 15, 14,  // RIGHT
        18, 17, 16, 16, 19, 18,  // TOP
        20, 21, 22, 22, 23, 20   // BOTTOM
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    int stride = 8 * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}