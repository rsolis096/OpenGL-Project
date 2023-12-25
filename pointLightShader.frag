#version 330 core


//Object color properties
struct Object
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform Object object;
uniform bool hasTexture;


void main()
{
    vec3 ambientColor = object.ambient; 
    vec3 diffuseColor = object.diffuse;
    vec3 specularColor = object.specular;
    vec3 result = ambientColor + diffuseColor + specularColor;
    FragColor = vec4(result, 1.0);
}


