#pragma once
#include "ServiceLocator.hpp"
#include <vector>
#include <algorithm>

// Forward declaration
class GameObject;

class GameObjectManager : public IService {
    friend class ServiceLocator;
public:
    
    
    void Register(GameObject* obj);
    void Unregister(GameObject* obj);
    void UpdateAll(double deltaTime);

private:
    GameObjectManager() = default;
    std::vector<GameObject*> objects;
    
    // Helper to give internal access if needed, though objects variable is now accessible to members
    std::vector<GameObject*>& GetObjects() { return objects; }
};