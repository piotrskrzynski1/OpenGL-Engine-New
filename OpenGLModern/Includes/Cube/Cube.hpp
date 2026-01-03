#pragma once
#include "Engine/GameObject.hpp"
#include <OPENGL/glm/glm.hpp>
#include <memory>

// Forward declarations
class Shader;
class Texture;

class Cube : public GameObject {
public:
    // Constructor with position parameter (default = origin)
    Cube(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));

    // Destructor
    ~Cube() override;

    // Update method (called every frame)
    void Update(double deltaTime) override;

    // Position getters/setters
    void SetPosition(const glm::vec3& pos) { position = pos; }
    glm::vec3 GetPosition() const { return position; }

private:
    // OpenGL buffers
    unsigned int VAO, VBO, EBO;

    // Rendering resources
    std::unique_ptr<Shader> shader;
    std::unique_ptr<Texture> texture;

    // Initialize mesh data
    void InitMesh();
};