#pragma once
#include <OPENGL/glm/glm.hpp>
#include <OPENGL/glm/gtc/matrix_transform.hpp>

class RenderContext;

class Camera {
public:
    Camera(glm::vec3 startPosition = glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3 startUp = glm::vec3(0.0f, 1.0f, 0.0f),
        float startYaw = -90.0f,
        float startPitch = 0.0f);

    void Update(double deltaTime);

    // Getters
    glm::vec3 GetPosition() const { return position; }
    glm::vec3 GetFront() const { return front; }
    float GetFOV() const { return fov; }

    // Setters
    void SetPosition(const glm::vec3& pos) { position = pos; }
    void SetMovementSpeed(float speed) { movementSpeed = speed; }
    void SetMouseSensitivity(float sensitivity) { mouseSensitivity = sensitivity; }

private:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    float yaw;
    float pitch;
    float movementSpeed;
    float mouseSensitivity;
    float fov;

    void UpdateCameraVectors();
};