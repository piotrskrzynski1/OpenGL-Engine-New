// ==========================================
// STRUCTURES (Must match C++ LightManager)
// ==========================================

struct DirLight {
    vec3 direction;
    vec3 color;
    float intensity;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    
    // Attenuation
    float constant;
    float linear;
    float quadratic;
};

struct Material {
    sampler2D diffuse;   // Texture unit 0
    sampler2D specular;  // Texture unit 1
    float shininess;
};

// ==========================================
// UNIFORMS
// ==========================================

// Define maximums matching your C++ engine limits
#define MAX_DIR_LIGHTS 4
#define MAX_POINT_LIGHTS 16

uniform DirLight dirLights[MAX_DIR_LIGHTS];
uniform PointLight pointLights[MAX_POINT_LIGHTS];

uniform int numDirLights;
uniform int numPointLights;

// ==========================================
// CALCULATIONS
// ==========================================

// 1. Calculate Directional Light
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 albedoSpec, float specMapVal, float shininess) {
    vec3 lightDir = normalize(-light.direction);
    
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Specular shading (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    
    // Combine results
    vec3 ambient  = light.color * light.intensity * 0.1 * albedoSpec; // Low ambient base
    vec3 diffuse  = light.color * light.intensity * diff * albedoSpec;
    vec3 specular = light.color * light.intensity * spec * specMapVal;
    
    return (ambient + diffuse + specular);
}

// 2. Calculate Point Light
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedoSpec, float specMapVal, float shininess) {
    vec3 lightDir = normalize(light.position - fragPos);
    
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Specular shading (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    
    // Attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    // Combine results
    vec3 ambient  = light.color * light.intensity * 0.1 * albedoSpec;
    vec3 diffuse  = light.color * light.intensity * diff * albedoSpec;
    vec3 specular = light.color * light.intensity * spec * specMapVal;
    
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    
    return (ambient + diffuse + specular);
}

// ==========================================
// MAIN AGGREGATOR
// ==========================================

// Call this function in your main()
vec3 CalculateAllLights(vec3 normal, vec3 fragPos, vec3 viewPos, vec3 albedo, float specMap, float shininess) {
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 result = vec3(0.0);

    // Directional Lights
    for(int i = 0; i < numDirLights; i++) {
        result += CalcDirLight(dirLights[i], norm, viewDir, albedo, specMap, shininess);
    }

    // Point Lights
    for(int i = 0; i < numPointLights; i++) {
        result += CalcPointLight(pointLights[i], norm, fragPos, viewDir, albedo, specMap, shininess);
    }
    
    return result;
}