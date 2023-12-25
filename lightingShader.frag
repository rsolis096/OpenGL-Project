#version 330 core
out vec4 FragColor;

//uniform sampler2D texture_diffuse1;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_height1;

struct Material {
    float shininess;
}; 

struct DirLight {
    vec3 direction;	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;   
    float constant;
    float linear;
    float quadratic;	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;  
    float constant;
    float linear;
    float quadratic;  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

//Only used for no texture primitives/models
struct Object
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 1
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;

uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

uniform Material material;
uniform Object object;
uniform bool hasTexture;


// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    

    //TO DO: PUT THESE RESULTS IN IF STATEMENTS DEPENDING ON WHICH LIGHT IS ON

    // phase 1: directional lighting
    //vec3 result = CalcDirLight(dirLight, norm, viewDir);
    // phase 2: point lights
    //for(int i = 0; i < NR_POINT_LIGHTS; i++)
        //result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
    //phase 3: spot light
    vec3 result = CalcSpotLight(spotLight, norm, FragPos, viewDir);    
        
    FragColor = vec4(result, 1.0);
}



// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    if(hasTexture)
    {
        //Diffuse (Blinn Phong)
        vec3 lightDir = normalize(light.position - fragPos);
        float lambertian = max(dot(normal, lightDir), 0.0);
        float specular = 0.0;

        if(lambertian > 0.0f)
        {
            vec3 halfDir = normalize(lightDir + viewDir);
            float specAngle = max(dot(halfDir, normal), 0.0);
            specular = pow(specAngle, material.shininess);
        }
                
        //Calculate Ambient, Diffuse, Specular
        vec3 ambientColor = light.ambient * vec3(texture(texture_diffuse1, TexCoords)); 
        vec3 diffuseColor = light.diffuse * lambertian * vec3(texture(texture_diffuse1, TexCoords)) * object.diffuse;
        vec3 specularColor = light.specular * specular * vec3(texture(texture_specular1, TexCoords));

        //Apply attenuation
        float distance = length(light.position - fragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
        ambientColor *= attenuation;
        diffuseColor *= attenuation;
        specularColor *= attenuation;

        //return contribution
        return (ambientColor + diffuseColor + specularColor);
    }
    else
    {
        //Diffuse (Blinn Phong)
        vec3 lightDir = normalize(light.position - fragPos);
        float lambertian = max(dot(normal, lightDir), 0.0);
        float specular = 0.0;

        if(lambertian > 0.0f)
        {
            vec3 halfDir = normalize(lightDir + viewDir);
            float specAngle = max(dot(halfDir, normal), 0.0);
            specular = pow(specAngle, material.shininess);
        }
                
        //Calculate Ambient, Diffuse, Specular
        vec3 ambientColor = light.ambient * object.ambient; 
        vec3 diffuseColor = light.diffuse * lambertian * object.diffuse;
        vec3 specularColor = light.specular * specular * object.specular;

        //Apply attenuation
        float distance = length(light.position - fragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
        ambientColor *= attenuation;
        diffuseColor *= attenuation;
        specularColor *= attenuation;

        //return contribution
        return (ambientColor + diffuseColor + specularColor);
    }
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    if(hasTexture)
    {
        //Diffuse (Blinn Phong)
        vec3 lightDir = normalize(light.position - fragPos);
        float lambertian = max(dot(normal, lightDir), 0.0);
        float specular = 0.0;

        if(lambertian > 0.0f)
        {
            vec3 halfDir = normalize(lightDir + viewDir);
            float specAngle = max(dot(halfDir, normal), 0.0);
            specular = pow(specAngle, material.shininess);
        }

        // Calculate the spotlight direction
        vec3 spotDir = normalize(light.direction);

        // Calculate the spotlight factor based on cone angles
        float spotAngle = dot(-lightDir, spotDir);
        float spotFactor = smoothstep(light.outerCutOff, light.cutOff, spotAngle);

        //Calculate Ambient
        vec3 ambientColor = light.ambient * vec3(texture(texture_diffuse1, TexCoords));

        // Calculate the diffuse component
        float diff = max(dot(Normal, lightDir), 0.0);
        vec3 diffuseColor = light.diffuse * vec3(texture(texture_diffuse1, TexCoords)) * (lambertian * spotFactor) * object.diffuse;

        // Calculate the specular component
        vec3 specularColor = light.specular * vec3(texture(texture_specular1, TexCoords)) * (specular * spotFactor);

        // Calculate the attenuation factor
        float distance = length(light.position - fragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

        return (ambientColor + diffuseColor + specularColor) * attenuation;
    }
    else
    {        
        //Diffuse (Blinn Phong)
        vec3 lightDir = normalize(light.position - fragPos);
        float lambertian = max(dot(normal, lightDir), 0.0);
        float specular = 0.0;

        if(lambertian > 0.0f)
        {
            vec3 halfDir = normalize(lightDir + viewDir);
            float specAngle = max(dot(halfDir, normal), 0.0);
            specular = pow(specAngle, material.shininess);
        }

        // Calculate the spotlight direction
        vec3 spotDir = normalize(light.direction);

        // Calculate the spotlight factor based on cone angles
        float spotAngle = dot(-lightDir, spotDir);
        float spotFactor = smoothstep(light.outerCutOff, light.cutOff, spotAngle);

        //Calculate Ambient
        vec3 ambientColor = light.ambient * object.diffuse;

        // Calculate the diffuse component
        float diff = max(dot(Normal, lightDir), 0.0);
        vec3 diffuseColor = light.diffuse * object.diffuse * (lambertian * spotFactor);

        // Calculate the specular component
        vec3 specularColor = light.specular * object.specular * (specular * spotFactor);

        // Calculate the attenuation factor
        float distance = length(light.position - fragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

        return (ambientColor + diffuseColor + specularColor) * attenuation;
    }
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    if(hasTexture)
    {
        //Diffuse (Blinn Phong)
        vec3 lightDir = normalize(-light.direction);
        float lambertian = max(dot(normal, lightDir), 0.0);
        float specular = 0.0;

        if(lambertian > 0.0f)
        {
            vec3 halfDir = normalize(lightDir + viewDir);
            float specAngle = max(dot(halfDir, normal), 0.0);
            specular = pow(specAngle, material.shininess);
        }
                
        //Calculate Ambient, Diffuse, Specular
        vec3 ambientColor = light.ambient * vec3(texture(texture_diffuse1, TexCoords)); 
        vec3 diffuseColor = light.diffuse * lambertian * vec3(texture(texture_diffuse1, TexCoords)) * object.diffuse;
        vec3 specularColor = light.specular * specular * vec3(texture(texture_specular1, TexCoords));

        //return contribution
        return (ambientColor + diffuseColor + specularColor);

    }
    else
    {
        //Diffuse (Blinn Phong)
        vec3 lightDir = normalize(-light.direction);
        float lambertian = max(dot(normal, lightDir), 0.0);
        float specular = 0.0;

        if(lambertian > 0.0f)
        {
            vec3 halfDir = normalize(lightDir + viewDir);
            float specAngle = max(dot(halfDir, normal), 0.0);
            specular = pow(specAngle, material.shininess);
        }
                
        //Calculate Ambient, Diffuse, Specular
        vec3 ambientColor = light.ambient * object.ambient; 
        vec3 diffuseColor = light.diffuse * lambertian * object.diffuse;
        vec3 specularColor = light.specular * specular * object.specular;

        //return contribution
        return (ambientColor + diffuseColor + specularColor);
    }

}
