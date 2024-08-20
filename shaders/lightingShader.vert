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
} vs_out;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrices[MAX_NR_SHADOW_MAPS];
uniform int numberOfSpotLightsVERT;

//Point Light = 0, Spot Light = 1, Directional Light = 2
uniform int lightType; 

void main()
{

    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    vs_out.TexCoords = aTexCoords;

    //Calculate the light space position of the fragment for SpotLights

        for(int i = 0; i < numberOfSpotLightsVERT; i++)
        {
            vs_out.FragPosLightSpace[i] = lightSpaceMatrices[i] * vec4(vs_out.FragPos, 1.0);
        }
    
    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
};