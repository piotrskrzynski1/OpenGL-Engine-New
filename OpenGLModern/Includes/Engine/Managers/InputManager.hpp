#pragma once
#include "ServiceLocator.hpp"
#include <GLFW/glfw3.h>

class InputManager : public IService {
friend class ServiceLocator;
public:
    
    // We disable copying/moving to prevent callback confusion
    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;

    // Helper for C-style callbacks to find this instance
    static InputManager* callbackInstance;

    void Initialize(GLFWwindow* window) {
        this->window = window;
        callbackInstance = this; // Set the static pointer for callbacks

        // Set up callbacks
        glfwSetKeyCallback(window, KeyCallback);
        glfwSetCursorPosCallback(window, MouseCallback);
        glfwSetMouseButtonCallback(window, MouseButtonCallback);
        glfwSetScrollCallback(window, ScrollCallback);

        // Capture mouse for FPS-style control
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // Initialize mouse position
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        lastMouseX = xpos;
        lastMouseY = ypos;
        firstMouse = true;
    }

    // Keyboard
    bool IsKeyPressed(int key) const {
        return glfwGetKey(window, key) == GLFW_PRESS;
    }

    bool IsKeyReleased(int key) const {
        return glfwGetKey(window, key) == GLFW_RELEASE;
    }

    // Mouse position delta
    float GetMouseDeltaX() const { return mouseDeltaX; }
    float GetMouseDeltaY() const { return mouseDeltaY; }

    // Mouse buttons
    bool IsMouseButtonPressed(int button) const {
        return glfwGetMouseButton(window, button) == GLFW_PRESS;
    }

    // Scroll wheel
    float GetScrollDelta() const { return scrollDelta; }

    // Reset deltas (call once per frame)
    void ResetDeltas() {
        mouseDeltaX = 0.0f;
        mouseDeltaY = 0.0f;
        scrollDelta = 0.0f;
    }

    // Toggle mouse capture
    void SetMouseCaptured(bool captured) {
        if (captured) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

private:
    InputManager() = default;
    GLFWwindow* window = nullptr;

    // Mouse state
    double lastMouseX = 0.0;
    double lastMouseY = 0.0;
    float mouseDeltaX = 0.0f;
    float mouseDeltaY = 0.0f;
    bool firstMouse = true;
    float scrollDelta = 0.0f;

    // --- Static Callbacks that route to the instance ---
    
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        // Can be used for events if needed
    }

    static void MouseCallback(GLFWwindow* window, double xpos, double ypos) {
        if (callbackInstance) {
            InputManager& input = *callbackInstance;

            if (input.firstMouse) {
                input.lastMouseX = xpos;
                input.lastMouseY = ypos;
                input.firstMouse = false;
            }

            input.mouseDeltaX = static_cast<float>(xpos - input.lastMouseX);
            input.mouseDeltaY = static_cast<float>(input.lastMouseY - ypos); 

            input.lastMouseX = xpos;
            input.lastMouseY = ypos;
        }
    }

    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
        // Can be used for events
    }

    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
        if (callbackInstance) {
            callbackInstance->scrollDelta = static_cast<float>(yoffset);
        }
    }
};

// Define the static member
inline InputManager* InputManager::callbackInstance = nullptr;