#pragma once
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <stdexcept>
#include <string>
#include <iostream>

// Base interface for all services
class IService {
public:
    virtual ~IService() = default;
};

class ServiceLocator {
public:
    // Global Access Point
    static ServiceLocator& Get() {
        static ServiceLocator instance;
        return instance;
    }

    ServiceLocator(const ServiceLocator&) = delete;
    ServiceLocator& operator=(const ServiceLocator&) = delete;

    // Register a service
    template <typename T>
    void Provide(std::shared_ptr<T> service) {
        services_[std::type_index(typeid(T))] = service;
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