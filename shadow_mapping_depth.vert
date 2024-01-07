#version 330 core
layout (location = 0) in vec3 aPos;

const int NR_SHADOW_MAPS = 4;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;
uniform int numberOfShadowMaps;

void main()
{
    //for(int i = 0; i < numberOfShadowMaps; i++)
        //lightSpaceMatrix *= lightSpaceMatrices[i];

    //lightSpaceMatrix *= lightSpaceMatrices[0];


    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}