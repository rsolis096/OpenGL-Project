#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos; 
in vec2 TexCoords;

uniform sampler2D texture1;
uniform bool hasTexture;

uniform vec3 lightPos; 
uniform vec3 objectColor;
uniform vec3 viewPos; 
uniform vec3 lightColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;  
  
uniform Material material;

void main()
{
    if(hasTexture)
	{
		// ambient
		float ambientStrength = 0.5;
		vec3 ambient = material.ambient * light.ambient * texture(texture1, TexCoords).rgb;
  	
		// diffuse 
		vec3 norm = normalize(Normal);
		vec3 lightDir = normalize(lightPos - FragPos);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = light.diffuse * (diff * material.diffuse) * texture(texture1, TexCoords).rgb;

		// specular
		float specularStrength = 0.5;
		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);  
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
		vec3 specular = light.specular * (spec * material.specular) * texture(texture1, TexCoords).rgb;  

		vec3 result = (ambient + diffuse + specular) * vec3(1.0f);

		FragColor = vec4(result, 1.0);
	}
	else
	{
		// ambient
		vec3 ambient = material.ambient * light.ambient;
  	
		// diffuse 
		vec3 norm = normalize(Normal);
		vec3 lightDir = normalize(lightPos - FragPos);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = light.diffuse * (diff * material.diffuse);

		// specular
		float specularStrength = 0.5;
		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);  
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		vec3 specular = light.specular * (spec * material.specular);  

		vec3 result = (ambient + diffuse + specular);

	    FragColor = vec4(result, 1.0);
	}

};