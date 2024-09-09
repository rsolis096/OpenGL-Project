#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

const int MAX_NR_SHADOW_MAPS = 8;
const int MAX_NR_POINT_LIGHTS = 8;
const int MAX_NR_SPOT_LIGHTS = 8;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace[MAX_NR_SHADOW_MAPS];
    vec4 dirFragPosLightSpace;
} vs_out;

// Uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
//uniform mat4 lightSpaceMatrices[MAX_NR_SHADOW_MAPS];
uniform bool hasDirLight; 
uniform mat4 directionalLightSpaceMatrix;

//Consider making a smaller struct for spotlights
struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;  
    float constant;
    float linear;
    float far_plane;
    float quadratic;  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
    sampler2D shadowMap;
    float bias;
    mat4 lightSpaceMatrix; //We only use this in vertex shader 
};

uniform SpotLight spotLights[MAX_NR_SPOT_LIGHTS];
uniform int numberOfSpotLights;

void main()
{
    //Process all lights
    vec4 worldPosition = model * vec4(aPos, 1.0);
    vs_out.FragPos = vec3(worldPosition);
    vs_out.Normal = normalize(transpose(inverse(mat3(model))) * aNormal);
    vs_out.TexCoords = aTexCoords;
    gl_Position = projection * view * worldPosition;
    
    // Process Spot Lights
    for (int i = 0; i < numberOfSpotLights; i++)
    {
        vs_out.FragPosLightSpace[i] = spotLights[i].lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    }
    
    //Process Directional Light
    if (hasDirLight)
    {
        vs_out.dirFragPosLightSpace = directionalLightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    }
}
