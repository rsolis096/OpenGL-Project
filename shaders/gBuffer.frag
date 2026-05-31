#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

// Used if a texture is present
struct Material
{
    float shininess;
    sampler2D ambient;
    sampler2D diffuse;
    sampler2D specular;
};

// Object color properties
struct Object
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in VS_OUT
{
    vec3 FragPosWorld;
    vec3 NormalWorld;
    vec2 TexCoords;
} fs_in;

uniform Material material;
uniform Object object;
uniform bool hasTexture;

void main()
{
    vec3 diffuseColor;
    vec3 specularColor;

    if (hasTexture)
    {
        diffuseColor = texture(material.diffuse, fs_in.TexCoords).rgb;
        specularColor = texture(material.specular, fs_in.TexCoords).rgb;
    }
    else
    {
        diffuseColor = object.diffuse;
        specularColor = object.specular;
    }

    // Attachment 0: world-space position
    gPosition = fs_in.FragPosWorld;

    // Attachment 1: world-space normal
    // Stored directly as float values, not remapped to [0, 1].
    gNormal = normalize(fs_in.NormalWorld);

    // Attachment 2:
    // RGB = diffuse/albedo
    // A   = simple specular strength
    float specularStrength = dot(specularColor, vec3(0.333333));

    gAlbedoSpec = vec4(diffuseColor, specularStrength);
}