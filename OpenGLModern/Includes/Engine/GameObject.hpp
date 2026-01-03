#pragma once
#include <Engine/Managers/ServiceLocator.hpp>
#include <Engine/Managers/GameObjectManager.hpp>
#include <OPENGL/glm/glm.hpp>
#include <OPENGL/glm/gtc/matrix_transform.hpp>

class GameObject {
protected:
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::vec3 rotation{0.0f, 0.0f, 0.0f};

public:
    // CONSTRUCTOR: Auto-registers via ServiceLocator
    GameObject() {
        auto manager = ServiceLocator::Get().GetService<GameObjectManager>();
        manager->Register(this);
    }

    // DESTRUCTOR: Auto-unregisters via ServiceLocator
    virtual ~GameObject() {
        try {
            auto manager = ServiceLocator::Get().GetService<GameObjectManager>();
            manager->Unregister(this);
        } catch (...) {
            // Locator might be destroyed at shutdown before objects; ignore
        }
    }

    GameObject(const GameObject&) = delete;
    GameObject& operator=(const GameObject&) = delete;

    virtual void Update(double deltaTime) = 0;
};