#version 330 core
out vec4 FragColor;

//Shader for skybox

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, TexCoords);
}