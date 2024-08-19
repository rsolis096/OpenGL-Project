#version 330 core
in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

uniform int lightType; //0 = Point Light, 1 = Spot Light

void main()
{
    if(lightType == 0){
        float lightDistance = length(FragPos.xyz - lightPos);
    
        // map to [0;1] range by dividing by far_plane
        lightDistance = lightDistance / far_plane;
    
        // write this as modified depth
        gl_FragDepth = lightDistance;
    }

}