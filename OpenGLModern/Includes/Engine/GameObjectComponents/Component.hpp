#pragma once
// REMOVE THIS: #include <Engine/GameObject.hpp> 

#include <iostream>
// Keep this if Component uses Shader methods, otherwise forward declare Shader too
#include <Shader/Shader.hpp> 

// Forward Declaration breaks the circle
class GameObject; 

class Component {
protected:
    GameObject* owner = nullptr;

public:
    virtual ~Component() = default;

    // We can keep the implementation here ONLY if we don't access members of GameObject.
    // Since we only store the pointer, this is valid.
    void SetOwner(GameObject* entity) {
        owner = entity;
    }

    GameObject* GetOwner() const { return owner; }

    virtual void Start() {} 
    virtual void Update(double deltaTime) {}
    virtual void Draw(Shader& shader) {} 
};