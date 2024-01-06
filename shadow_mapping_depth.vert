#version 330 core
layout (location = 0) in vec3 aPos;

const int NR_SHADOW_MAPS = 4;

uniform mat4 lightSpaceMatrices[NR_SHADOW_MAPS];
uniform mat4 model;
uniform int numberOfShadowMaps;

void main()
{
    mat4 lightSpaceMatrix = mat4(1.0);

    for(int i = 0; i < numberOfShadowMaps; i++)
        lightSpaceMatrix *= lightSpaceMatrices[i];

    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}