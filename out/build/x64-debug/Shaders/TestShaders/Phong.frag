#version 330 core
out vec4 FragColor;

// --- Inputs from Vertex Shader ---
in vec3 FragPos;  
in vec3 Normal;   // READ-ONLY input
in vec2 TexCoord;

// --- Uniforms ---
uniform sampler2D texture1;
uniform vec3 viewPos; 

// NOTE: Lighting.glsl is injected here automatically
// It provides: CalculateAllLights(...)

void main()
{

    vec3 norm = normalize(Normal); 

    // 1. Get the base color from your texture
    vec4 texColor = texture(texture1, TexCoord);
    
    // 2. Setup Material Properties
    vec3 albedo = texColor.rgb;
    float specMap = 0.5;   
    float shininess = 32.0; 

    // 3. Calculate Lighting
    // PASS 'norm' HERE instead of 'Normal'
    vec3 result = CalculateAllLights(norm, FragPos, viewPos, albedo, specMap, shininess);

    FragColor = vec4(result, 1.0);
}