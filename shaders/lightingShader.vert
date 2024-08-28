#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

const int MAX_NR_SHADOW_MAPS = 8;

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
uniform mat4 lightSpaceMatrices[MAX_NR_SHADOW_MAPS];
uniform int numberOfSpotLights;
uniform bool hasDirLight; 
uniform mat4 directionalLightSpaceMatrix;

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
        vs_out.FragPosLightSpace[i] = lightSpaceMatrices[i] * vec4(vs_out.FragPos, 1.0);
    }
    
    //Process Directional Light
    if (hasDirLight)
    {
        vs_out.dirFragPosLightSpace = directionalLightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    }
}
