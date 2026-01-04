#include <OPENGL/glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <memory>

#include <Engine/Managers/ServiceLocator.hpp>
#include <Engine/Managers/GameObjectManager.hpp>
#include <Engine/Managers/InputManager.hpp>
#include <Engine/RenderContext.hpp>
#include <Cube/Cube.hpp> 
#include "GameObjects/Camera/Camera.hpp"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    
    // Safety check if RenderContext is initialized
    try {
        auto render = ServiceLocator::Get().GetService<RenderContext>();
        render->SetPerspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
    } catch(...) {
        // RenderContext might not be ready yet
    }
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    // --- 1. GLFW/GLAD Initialization ---
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Service Locator Engine", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // --- 2. SERVICE REGISTRATION (Composition Root) ---
    
    // Create the services
    auto inputSystem = ServiceLocator::Get().Create<InputManager>();
    auto renderSystem = ServiceLocator::Get().Create<RenderContext>();
    auto objectSystem = ServiceLocator::Get().Create<GameObjectManager>();

    // Initialize the services
    inputSystem->Initialize(window);
    renderSystem->SetPerspective(45.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    // --- 3. GAME SETUP ---

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    std::cout << "Engine started...\n";

    // Create a Cube (It will auto-register because GameObjectManager is now in the Locator)
    auto cube1 = std::make_shared<Cube>(glm::vec3(0.0f, 0.0f, 0.0f));

    // Add Lights
    //renderSystem->GetLightManager().addPointLight(glm::vec3(0.0f, 1.5f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    // --- 4. GAME LOOP ---
    double lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        double currentFrame = glfwGetTime();
        double deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update Camera
        auto camera = renderSystem->GetMainCamera();
        if (camera) {
            camera->Update(deltaTime);
        }

        // Update All GameObjects
        objectSystem->UpdateAll(deltaTime);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}