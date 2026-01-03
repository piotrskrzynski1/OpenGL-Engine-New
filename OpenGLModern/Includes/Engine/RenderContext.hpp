#pragma once
#include <Engine/Managers/ServiceLocator.hpp>
#include <memory>
#include <OPENGL/glm/glm.hpp>
#include <Engine/Managers/LightManager.hpp>

class Camera; // Forward declaration

class RenderContext : public IService {
public:
    RenderContext();
    
    RenderContext(const RenderContext&) = delete;
    RenderContext& operator=(const RenderContext&) = delete;

    // --- Getters ---
    const glm::mat4& GetViewMatrix() const;
    const glm::mat4& GetProjectionMatrix() const;
    LightManager& GetLightManager();
    Camera* GetMainCamera();

    // --- Setters ---
    void SetViewMatrix(const glm::mat4& v);
    void SetProjectionMatrix(const glm::mat4& p);
    void SetLightManager(const LightManager& lm);
    void SetPerspective(float fov, float aspect, float nearPlane, float farPlane);

private:
    std::unique_ptr<Camera> mainCam;
    LightManager lightmanager;
    glm::mat4 view;
    glm::mat4 projection;
};