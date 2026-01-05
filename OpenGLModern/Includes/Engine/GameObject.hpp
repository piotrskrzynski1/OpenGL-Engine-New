#pragma once
#include <Engine/Managers/ServiceLocator.hpp>
#include <Engine/Managers/GameObjectManager.hpp>
#include <OPENGL/glm/glm.hpp>
#include <OPENGL/glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>
#include <algorithm>

// Include the Component definition because template methods (AddComponent) need it
#include <Engine/GameObjectComponents/Component.hpp> 

// Forward Declaration for Shader to fix "syntax error: identifier 'Shader'"
class Shader; 

class GameObject {
protected:
    std::vector<std::unique_ptr<Component>> components;

public:
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::vec3 rotation{0.0f, 0.0f, 0.0f};
    glm::vec3 scale{1.0f, 1.0f, 1.0f}; 

    GameObject() {
        auto manager = ServiceLocator::Get().GetService<GameObjectManager>();
        manager->Register(this);
    }

    virtual ~GameObject() {
        try {
             auto manager = ServiceLocator::Get().GetService<GameObjectManager>();
             manager->Unregister(this);
        } catch(...) {}
    }

    // --- Component System ---
    template <typename T, typename... Args>
    T* AddComponent(Args&&... args) {
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        
        // This works because Component is fully defined by the include above
        component->SetOwner(this);
        component->Start();
        
        T* rawPtr = component.get();
        components.push_back(std::move(component));
        return rawPtr;
    }

    template <typename T>
    T* GetComponent() {
        for (auto& comp : components) {
            if (T* ptr = dynamic_cast<T*>(comp.get())) {
                return ptr;
            }
        }
        return nullptr;
    }

    virtual void Update(double deltaTime) {
        for (auto& comp : components) {
            comp->Update(deltaTime);
        }
    }
    
    // Shader is now known via forward declaration
    void Draw(Shader& shader) {
         for (auto& comp : components) {
            comp->Draw(shader);
        }
    }
};