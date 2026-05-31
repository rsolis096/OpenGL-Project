#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

// 0 = albedo
// 1 = normal
// 2 = world position
// 3 = specular strength
// 4 = position length/depth-ish
uniform int debugMode;

// Used only for visualizing world position.
// Try values like 10, 20, 50, 100 depending on scene scale.
uniform float positionDebugScale;

void main()
{
    vec3 position = texture(gPosition, TexCoords).rgb;
    vec3 normal = normalize(texture(gNormal, TexCoords).rgb);
    vec4 albedoSpec = texture(gAlbedoSpec, TexCoords);

    if (debugMode == 0)
    {
        // Albedo / flat material color
        FragColor = vec4(albedoSpec.rgb, 1.0);

    }
    else if (debugMode == 1)
    {
        // Normal debug.
        // Normals are stored as [-1, 1], so remap to visible [0, 1].
        FragColor = vec4(normal * 0.5 + 0.5, 1.0);
    }
    else if (debugMode == 2)
    {
        // World position debug.
        // World positions are not naturally colors, so remap them.
        vec3 visualPosition = position / positionDebugScale + 0.5;
        FragColor = vec4(visualPosition, 1.0);
    }
    else if (debugMode == 3)
    {
        // Specular strength debug
        float specularStrength = albedoSpec.a;
        FragColor = vec4(vec3(specularStrength), 1.0);
    }
    else if (debugMode == 4)
    {
        // Distance from world origin. Useful for checking that gPosition is sane.
        float distanceFromOrigin = length(position);
        float visualDistance = distanceFromOrigin / positionDebugScale;
        FragColor = vec4(vec3(visualDistance), 1.0);
    }
    else
    {
        // Fallback: bright magenta means invalid debug mode
        FragColor = vec4(1.0, 0.0, 1.0, 1.0);
    }
}