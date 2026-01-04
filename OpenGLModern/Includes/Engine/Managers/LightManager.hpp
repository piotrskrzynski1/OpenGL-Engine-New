#pragma once

#include <vector>
#include <memory>
#include <string>
#include <algorithm>

// Your specific includes
#include <OPENGL/glm/glm.hpp>
#include <Shader/Shader.hpp> // Assuming this is where your Shader class lives
#include <Engine/Managers/ServiceLocator.hpp>
// ==========================================
// Light Types
// ==========================================

enum class LightType {
    Directional,
    Point,
    Spot
};

// ==========================================
// Abstract Base Class
// ==========================================

class Light {
public:
    Light(const glm::vec3& color, float intensity)
        : color(color), intensity(intensity) {
    }

    virtual ~Light() = default;
    virtual LightType getType() const = 0;

    glm::vec3 color;
    float intensity;
    bool enabled = true;
};

// ==========================================
// Directional Light
// ==========================================

class DirectionalLight : public Light {
public:
    DirectionalLight(const glm::vec3& dir, const glm::vec3& color, float intensity = 1.0f)
        : Light(color, intensity), direction(glm::normalize(dir)) {
    }

    LightType getType() const override { return LightType::Directional; }

    glm::vec3 direction;
};

// ==========================================
// Point Light
// ==========================================

class PointLight : public Light {
public:
    PointLight(const glm::vec3& pos, const glm::vec3& color, float intensity = 1.0f)
        : Light(color, intensity), position(pos) {
    }

    LightType getType() const override { return LightType::Point; }

    glm::vec3 position;

    // Default attenuation values cover ~50 units of distance
    struct Attenuation {
        float constant = 1.0f;
        float linear = 0.09f;
        float quadratic = 0.032f;
    } attenuation;
};

class SpotLight : public Light {
public:
    // cutOffInDegrees: The angle of the inner cone (full brightness)
    // outerCutOffInDegrees: The angle of the outer cone (fades to darkness)
    SpotLight(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& color, 
              float cutOffInDegrees, float outerCutOffInDegrees, float intensity = 1.0f)
        : Light(color, intensity), position(pos), direction(glm::normalize(dir)) 
    {
        // Convert degrees to Cosine values for efficient shader calculation
        cutOff = glm::cos(glm::radians(cutOffInDegrees));
        outerCutOff = glm::cos(glm::radians(outerCutOffInDegrees));
    }

    LightType getType() const override { return LightType::Spot; }

    glm::vec3 position;
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;

    struct Attenuation {
        float constant = 1.0f;
        float linear = 0.09f;
        float quadratic = 0.032f;
    } attenuation;
};

// ==========================================
// Light Manager
// ==========================================

class LightManager : public IService {

    friend class ServiceLocator;
public:
    ~LightManager() = default;

    // --- Add Lights ---

    std::shared_ptr<DirectionalLight> addDirectionalLight(const glm::vec3& dir, const glm::vec3& color, float intensity = 1.0f) {
        auto light = std::make_shared<DirectionalLight>(dir, color, intensity);
        dirLights.push_back(light);
        return light;
    }

    std::shared_ptr<PointLight> addPointLight(const glm::vec3& pos, const glm::vec3& color, float intensity = 1.0f) {
        auto light = std::make_shared<PointLight>(pos, color, intensity);
        pointLights.push_back(light);
        return light;
    }

    std::shared_ptr<SpotLight> addSpotLight(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& color, 
                                            float cutOffDeg = 12.5f, float outerCutOffDeg = 15.0f, float intensity = 1.0f) {
        auto light = std::make_shared<SpotLight>(pos, dir, color, cutOffDeg, outerCutOffDeg, intensity);
        spotLights.push_back(light);
        return light;
    }

    void clear() {
        dirLights.clear();
        pointLights.clear();
        spotLights.clear();
    }

    // --- Main Registration Function ---

    // Call this inside your render loop, right after shader.use()
    void UpdateShader(const Shader& shader) const {

        // 1. Process Directional Lights
        int activeDirLights = 0;
        for (const auto& light : dirLights) {
            if (!light->enabled) continue;

            // Limit to prevent array overflow in shader
            if (activeDirLights >= 4) break;

            std::string base = "dirLights[" + std::to_string(activeDirLights) + "]";

            shader.setVec3(base + ".direction", light->direction);
            shader.setVec3(base + ".color", light->color);
            shader.setFloat(base + ".intensity", light->intensity);

            activeDirLights++;
        }
        shader.setInt("numDirLights", activeDirLights);

        // 2. Process Point Lights
        int activePointLights = 0;
        for (const auto& light : pointLights) {
            if (!light->enabled) continue;

            // Limit to prevent array overflow in shader
            if (activePointLights >= 16) break;

            std::string base = "pointLights[" + std::to_string(activePointLights) + "]";

            shader.setVec3(base + ".position", light->position);
            shader.setVec3(base + ".color", light->color);
            shader.setFloat(base + ".intensity", light->intensity);

            shader.setFloat(base + ".constant", light->attenuation.constant);
            shader.setFloat(base + ".linear", light->attenuation.linear);
            shader.setFloat(base + ".quadratic", light->attenuation.quadratic);

            activePointLights++;
        }

        shader.setInt("numPointLights", activePointLights);

        int activeSpotLights = 0;
        for (const auto& light : spotLights) {
            if (!light->enabled) continue;

            // Limit to prevent shader overflow (Matching defines in GLSL)
            if (activeSpotLights >= 4) break; 

            std::string base = "spotLights[" + std::to_string(activeSpotLights) + "]";

            shader.setVec3(base + ".position", light->position);
            shader.setVec3(base + ".direction", light->direction);
            shader.setVec3(base + ".color", light->color);
            shader.setFloat(base + ".intensity", light->intensity);

            shader.setFloat(base + ".cutOff", light->cutOff);
            shader.setFloat(base + ".outerCutOff", light->outerCutOff);

            shader.setFloat(base + ".constant", light->attenuation.constant);
            shader.setFloat(base + ".linear", light->attenuation.linear);
            shader.setFloat(base + ".quadratic", light->attenuation.quadratic);

            activeSpotLights++;
        }
        shader.setInt("numSpotLights", activeSpotLights);
    }

private:

    LightManager() = default;
    std::vector<std::shared_ptr<DirectionalLight>> dirLights;
    std::vector<std::shared_ptr<PointLight>> pointLights;
    std::vector<std::shared_ptr<SpotLight>> spotLights;
};