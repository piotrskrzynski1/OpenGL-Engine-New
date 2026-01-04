#pragma once
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <stdexcept>
#include <string>
#include <iostream>

// 1. Ensure the base interface exists
class IService {
public:
    virtual ~IService() = default;
};

class ServiceLocator {
public:
    static ServiceLocator& Get() {
        static ServiceLocator instance;
        return instance;
    }

    ServiceLocator(const ServiceLocator&) = delete;
    ServiceLocator& operator=(const ServiceLocator&) = delete;

    // --- NEW: Create and Register a service in one step ---
    // This allows passing arguments to the constructor: Create<Camera>(pos, up, yaw, pitch);
    template <typename T, typename... Args>
    std::shared_ptr<T> Create(Args&&... args) {
        // We use 'new' directly because ServiceLocator will be a friend
        // and can access the private constructor.
        std::shared_ptr<T> service(new T(std::forward<Args>(args)...));
        
        services_[std::type_index(typeid(T))] = service;
        return service;
    }

    // Retrieve a service
    template <typename T>
    std::shared_ptr<T> GetService() {
        auto it = services_.find(std::type_index(typeid(T)));
        if (it == services_.end()) {
            throw std::runtime_error("Service not found: " + std::string(typeid(T).name()));
        }
        return std::static_pointer_cast<T>(it->second);
    }

private:
    ServiceLocator() = default;
    std::unordered_map<std::type_index, std::shared_ptr<IService>> services_;
};