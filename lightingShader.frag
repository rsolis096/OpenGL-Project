#version 330 core
out vec4 FragColor;
const int NR_POINT_LIGHTS = 8;
const int NR_SPOT_LIGHTS = 8;

//Shadow Map
uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

//Material Textures
//uniform sampler2D texture_diffuse1;
//uniform sampler2D texture_specular1;
//uniform sampler2D texture_normal1;
//uniform sampler2D texture_height1;


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
    float padding;
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

//Object color properties
struct Object
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform vec3 viewPos;
uniform vec3 lightPos;


uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform int numberOfPointLights;
uniform SpotLight spotLights[NR_SPOT_LIGHTS];
uniform int numberOfSpotLights;

uniform Material material;
uniform Object object;
uniform bool hasTexture;


// function prototypes
//vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
//vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

uniform float textureScale;  // Scaling factor for texture coordinates
vec2 scaledTexCoords;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    //Bias aims to solve light acne
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);  

    // PCF
    float shadow = 0.0;
    //Texel size, decrease the size of numerator to make shadows sharper
    vec2 texelSize = 0.8 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0; //Larger sample size can make the shadow edges softer
    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}


void main()
{
    
    // properties
    vec3 norm = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    scaledTexCoords = fs_in.TexCoords * 1;
    

    //TO DO: PUT THESE RESULTS IN IF STATEMENTS DEPENDING ON WHICH LIGHT IS ON

    // phase 1: directional lighting
    vec3 result;// = CalcDirLight(dirLight, norm, viewDir);
    // phase 2: point lights
    for(int i = 0; i < numberOfPointLights; i++)
    {
        //result += CalcPointLight(pointLights[i], norm, fs_in.FragPos, viewDir);           
    }
    //phase 3: spot light
    //for(int i = 0; i < numberOfSpotLights; i++)
    {
        result += CalcSpotLight(spotLights[0], norm, fs_in.FragPos, viewDir);           
    }
        
    FragColor = vec4(result, 1.0);
    
}

/*
// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{

    //Diffuse (Blinn Phong)
    vec3 lightDir = normalize(lightPos-fragPos);
    float lambertian = max(dot(normal, lightDir), 0.0);
    float specular = 0.0;

    if(lambertian > 0.0f)
    {
        vec3 halfDir = normalize(lightDir + viewDir);
        float specAngle = max(dot(halfDir, normal), 0.0);
        specular = pow(specAngle, 1.0f);
    }

    //Calculate Ambient, Diffuse, Specular
    vec3 ambientColor = light.ambient * object.ambient; 
    vec3 diffuseColor = light.diffuse * lambertian * object.diffuse;
    vec3 specularColor = light.specular * specular * object.specular;

    if(hasTexture)
    {              
        //Calculate Ambient, Diffuse, Specular
        ambientColor = light.ambient * vec3(texture(texture_diffuse1, scaledTexCoords)); 
        diffuseColor = light.diffuse * lambertian * vec3(texture(texture_diffuse1, scaledTexCoords)) * object.diffuse;
        specularColor = light.specular * specular * vec3(texture(texture_specular1, scaledTexCoords));
    }

    //Apply attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    ambientColor *= attenuation;
    diffuseColor *= attenuation;
    specularColor *= attenuation;


    //return contribution
    return (ambientColor + diffuseColor + specularColor);

}
*/

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    //Calculate Ambient
    vec3 ambientColor = light.ambient;

    // Calculate the diffuse component
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, fs_in.Normal), 0.0);
    vec3 diffuseColor = diff * light.diffuse;

    // Calculate specular component
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specularColor = spec * light.specular;  

    // Calculate the attenuation factor
    float distance = length(lightPos - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));


    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, normal, lightDir);        //The object texture v 
    
    // Apply shadow to diffuse and specular components separately
    vec3 diffuseWithShadow = (1.0 - shadow) * diffuseColor;
    vec3 specularWithShadow = (1.0 - shadow) * specularColor;

    vec3 lighting = (ambientColor + diffuseWithShadow + specularColor) * vec3(texture(diffuseTexture, scaledTexCoords));    
    
    return lighting * attenuation;
}

/*
// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
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

    //Calculate Ambient, Diffuse, Specular (no texture)
    vec3 ambientColor = light.ambient * object.ambient; 
    vec3 diffuseColor = light.diffuse * lambertian * object.diffuse;
    vec3 specularColor = light.specular * specular * object.specular;

    if(hasTexture)
    {          
        //Calculate Ambient, Diffuse, Specular
        ambientColor = light.ambient * vec3(texture(texture_diffuse1, scaledTexCoords)); 
        diffuseColor = light.diffuse * lambertian * vec3(texture(texture_diffuse1, scaledTexCoords)) * object.diffuse;
        specularColor = light.specular * specular * vec3(texture(texture_specular1, scaledTexCoords));
    }

    //return contribution
    return (ambientColor + diffuseColor + specularColor);
}
*/