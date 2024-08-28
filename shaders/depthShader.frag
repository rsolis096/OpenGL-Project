#version 330 core
in vec4 FragPos;

uniform vec3 pointLightPos;

// Maximum distance for depth normalization
uniform float far_plane; 

// 0 = Point Light, 1 = Spot Light, 2 = Directional Light
uniform int lightType;

void main()
{
    // Point Light
    if (lightType == 0) 
    { 
        // Calculate the distance from the fragment to the light position
        float lightDistance = length(FragPos.xyz - pointLightPos);
        
        // Normalize the distance by dividing by far_plane to fit depth in range [0, 1]
        lightDistance = lightDistance / far_plane;   
        
        // Set the depth value in the shadow map
        gl_FragDepth = lightDistance;
    }    

    // Spot Light or Directional Light
    else if (lightType == 1 || lightType == 2) 
    {
        // Use the default fragment depth for spot and directional lights
        gl_FragDepth = gl_FragCoord.z; 
    }
}
