#pragma once
#include "ServiceLocator.hpp"
#include <vector>
#include <algorithm>

// Forward declaration
class GameObject;

class GameObjectManager : public IService {
public:
    GameObjectManager() = default;
    
    void Register(GameObject* obj);
    void Unregister(GameObject* obj);
    void UpdateAll(double deltaTime);

private:
    std::vector<GameObject*> objects;
    
    // Helper to give internal access if needed, though objects variable is now accessible to members
    std::vector<GameObject*>& GetObjects() { return objects; }
};