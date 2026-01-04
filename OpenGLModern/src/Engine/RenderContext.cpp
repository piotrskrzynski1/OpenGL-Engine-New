#include "Engine/RenderContext.hpp"
#include "GameObjects/Camera/Camera.hpp" 
#include <OPENGL/glm/gtc/matrix_transform.hpp>

RenderContext::RenderContext()
    : view(1.0f),
    projection(1.0f),
    mainCam(nullptr)
{
    // FIX: Ask the ServiceLocator to create the LightManager.
    // This creates it, registers it, and gives us the pointer.
    lightmanager = ServiceLocator::Get().Create<LightManager>();
}

const glm::mat4& RenderContext::GetViewMatrix() const {
    return view;
}

const glm::mat4& RenderContext::GetProjectionMatrix() const {
    return projection;
}

LightManager& RenderContext::GetLightManager() {
    return *lightmanager; 
}

Camera* RenderContext::GetMainCamera() {
    if (!mainCam) {
        mainCam = std::make_unique<Camera>();
    }
    return mainCam.get();
}

void RenderContext::SetViewMatrix(const glm::mat4& v) {
    view = v;
}

void RenderContext::SetProjectionMatrix(const glm::mat4& p) {
    projection = p;
}

void RenderContext::SetLightManager(std::shared_ptr<LightManager> lm) {
    lightmanager = lm;
}

void RenderContext::SetPerspective(float fov, float aspect, float nearPlane, float farPlane) {
    projection = glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
}