#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos; 
in vec2 TexCoords;

uniform bool hasTexture;
uniform vec3 lightPos; 
uniform vec3 viewPos;

struct Object
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    //Implement Attenuation (quadratic light drop off)
    float constant;
    float linear;
    float quadratic;
};

uniform Light light;  
uniform Material material;
uniform Object object;

void main()
{

    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));   

    if(hasTexture)
	{
        // ambient
        vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
  	
        // diffuse 
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(light.position - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;  
    
        // specular
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;  

        ambient  *= attenuation; 
        diffuse  *= attenuation;
        specular *= attenuation;   
        
        vec3 result = ambient + diffuse + specular;
        FragColor = vec4(result, 1.0);
	}
	else
	{
    
        // ambient
        float ambientStrength = 0.1;
        vec3 ambient = ambientStrength * object.ambient;
  	
        // diffuse 
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(light.position - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = (object.diffuse * diff) * light.diffuse;
    
        // specular
        float specularStrength = 0.5;
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 specular = light.specular * (spec * object.specular);  

        ambient  *= attenuation; 
        diffuse  *= attenuation;
        specular *= attenuation;   
        
        vec3 result = (ambient + diffuse + specular);
        FragColor = vec4(result, 1.0);
	}
};