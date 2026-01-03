#include <Engine/Managers/GameObjectManager.hpp>
#include <Engine/GameObject.hpp>

void GameObjectManager::Register(GameObject* obj) {
    objects.push_back(obj);
}

void GameObjectManager::Unregister(GameObject* obj) {
    objects.erase(
        std::remove(objects.begin(), objects.end(), obj),
        objects.end()
    );
}

void GameObjectManager::UpdateAll(double deltaTime) {
    // Copy vector to handle objects being deleted during update
    auto objectsCopy = objects;

    for (auto* obj : objectsCopy) {
        // Verify object still exists in the live list
        bool exists = std::find(objects.begin(), objects.end(), obj) != objects.end();

        if (exists) {
            obj->Update(deltaTime);
        }
    }
}