#pragma once
#include "Engine/GameObject.hpp"
#include <OPENGL/glm/glm.hpp>
#include <memory>

#include <Engine/GameObjectComponents/MeshRenderer.hpp>

// Forward declarations
class Shader;
class Texture;

class Cube : public GameObject {
public:
    Cube(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));
    ~Cube() override;

    void Update(double deltaTime) override;

    void SetPosition(const glm::vec3& pos) { position = pos; }
    glm::vec3 GetPosition() const { return position; }

private:
    // The Cube now OWNS a MeshRenderer component to do the heavy lifting
    std::unique_ptr<MeshRenderer> meshRenderer;

    std::unique_ptr<Shader> shader;
    std::unique_ptr<Texture> texture;
};