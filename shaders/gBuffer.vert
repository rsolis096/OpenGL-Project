#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT
{
    vec3 FragPosWorld;
    vec3 NormalWorld;
    vec2 TexCoords;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPosition = model * vec4(aPos, 1.0);

    vs_out.FragPosWorld = worldPosition.xyz;

    // Same world-space normal transform style as your current forward shader.
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vs_out.NormalWorld = normalize(normalMatrix * aNormal);

    vs_out.TexCoords = aTexCoords;

    gl_Position = projection * view * worldPosition;
}