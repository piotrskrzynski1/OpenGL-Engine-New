#include "Gameobjects/Camera/Camera.hpp"
#include <Engine/Managers/ServiceLocator.hpp>
#include "Engine/RenderContext.hpp"
#include "Engine/Managers/InputManager.hpp"
#include <OPENGL/GLFW/glfw3.h>
#include <cmath>

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, float startYaw, float startPitch)
    : position(startPosition),
    worldUp(startUp),
    yaw(startYaw),
    pitch(startPitch),
    movementSpeed(2.5f),
    mouseSensitivity(0.1f),
    fov(45.0f)
{
    UpdateCameraVectors();
    
    // Set initial view matrix
    auto render = ServiceLocator::Get().GetService<RenderContext>();
    const glm::mat4 view = glm::lookAt(position, position + front, up);
    render->SetViewMatrix(view);
}

void Camera::UpdateCameraVectors() {
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);

    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

void Camera::Update(double deltaTime) {
    // 1. ACCESS SERVICES
    auto input = ServiceLocator::Get().GetService<InputManager>();
    auto render = ServiceLocator::Get().GetService<RenderContext>();

    // 2. MOVEMENT (WASD)
    float velocity = movementSpeed * (float)deltaTime;

    if (input->IsKeyPressed(GLFW_KEY_W))
        position += front * velocity;
    if (input->IsKeyPressed(GLFW_KEY_S))
        position -= front * velocity;
    if (input->IsKeyPressed(GLFW_KEY_A))
        position -= right * velocity;
    if (input->IsKeyPressed(GLFW_KEY_D))
        position += right * velocity;

    if (input->IsKeyPressed(GLFW_KEY_SPACE))
        position += up * velocity;
    if (input->IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
        position -= up * velocity;

    // 3. MOUSE LOOK
    float mouseDeltaX = input->GetMouseDeltaX();
    float mouseDeltaY = input->GetMouseDeltaY();

    yaw += mouseDeltaX * mouseSensitivity;
    pitch += mouseDeltaY * mouseSensitivity;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    // 4. ZOOM
    float scrollDelta = input->GetScrollDelta();
    if (scrollDelta != 0.0f) {
        fov -= scrollDelta;
        if (fov < 1.0f) fov = 1.0f;
        if (fov > 90.0f) fov = 90.0f;

        render->SetPerspective(fov, 800.0f / 600.0f, 0.1f, 100.0f);
    }

    UpdateCameraVectors();

    // 5. UPDATE RENDER CONTEXT
    glm::mat4 view = glm::lookAt(position, position + front, up);
    render->SetViewMatrix(view);

    input->ResetDeltas();
}