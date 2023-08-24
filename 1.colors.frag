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
    vec3 direction;


    //SpotLight Properties
    float cutOff;
    float outerCutOff;

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

uniform int lightFlag; //1 for PointLight, 2 - SpotLight

void main()
{

    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));  
   

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    if(lightFlag == 1)
    {
        if(hasTexture)
	    {
            // ambient
            ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
  	
            // diffuse 
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(light.position - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;  
    
            // specular
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);  
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
            specular = light.specular * spec * texture(material.specular, TexCoords).rgb;  
	    }
	    else
	    { 
            // ambient
            ambient = light.ambient * object.ambient;
  	
            // diffuse 
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(light.position - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            diffuse = (object.diffuse * diff) * light.diffuse;
    
            // specular
            float specularStrength = 0.5;
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);  
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
            specular = light.specular * (spec * object.specular);  
	    }

        
        ambient  *= attenuation; 
        diffuse  *= attenuation;
        specular *= attenuation;   
        
        vec3 result = (ambient + diffuse + specular);
        FragColor = vec4(result, 1.0);
        
    }
    else if(lightFlag == 2)
    {
        vec3 lightDir = normalize(light.position - FragPos);
        // check if lighting is inside the spotlight cone
        float theta = dot(lightDir, normalize(-light.direction)); 

        if(theta > light.cutOff) // remember that we're working with angles as cosines instead of degrees so a '>' is used.
        {    
            // ambient
            ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
        
            // diffuse 
            vec3 norm = normalize(Normal);
            float diff = max(dot(norm, lightDir), 0.0);
            diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;  
        
            // specular
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);  
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
            specular = light.specular * spec * texture(material.specular, TexCoords).rgb;  
        
            // attenuation
            float distance    = length(light.position - FragPos);
            attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));   

            // ambient  *= attenuation; // remove attenuation from ambient, as otherwise at large distances the light would be darker inside than outside the spotlight due the ambient term in the else branch
            diffuse   *= attenuation;
            specular *= attenuation;   
            
            vec3 result = ambient + diffuse + specular;
            FragColor = vec4(result, 1.0);
        }
        else 
        {
            // else, use ambient light so scene isn't completely dark outside the spotlight.
            FragColor = vec4(light.ambient * texture(material.diffuse, TexCoords).rgb, 1.0);
        }
    }

};