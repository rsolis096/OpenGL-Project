#version 330 core
out vec4 FragColor;

const int MAX_NR_POINT_LIGHTS = 8;
const int MAX_NR_SPOT_LIGHTS = 8;
const int MAX_NR_SHADOW_MAPS = 8;

//Shadow Map
uniform sampler2D shadowMap[MAX_NR_SHADOW_MAPS];

//Used if a texture is present
struct Material {
    float shininess;
    sampler2D ambient;
    sampler2D diffuse;
    sampler2D specular;
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

//Object color properties (used if texture is not available or if you want to change tint of texture)
struct Object
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

//Properties sent to fragment shader from vertex shader
in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace[MAX_NR_SHADOW_MAPS];
} fs_in;

//Player Position
uniform vec3 viewPos;

//Scene Light Objects
uniform DirLight dirLight;
uniform PointLight pointLights[MAX_NR_POINT_LIGHTS];
uniform int numberOfPointLights;
uniform SpotLight spotLights[MAX_NR_SPOT_LIGHTS];
uniform int numberOfSpotLightsFRAG;

//Fragment Properties
uniform Material material;
uniform Object object;
uniform bool hasTexture;

// function prototypes
//vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, int mapIndex);

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightPosition, int mapIndex)
{
    vec3 lightDirection = normalize(lightPosition - fragPosLightSpace.xyz);

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    vec3 shadowCoords = projCoords * 0.5 + vec3(0.5);

    float diffuseFactor = dot(fs_in.Normal, -lightDirection);
    float bias = mix(0.001, 0.0, diffuseFactor);

    float texelWidth = 1.0 / (1024*2);
    float texelHeight = 1.0 / (1024*2);

    vec2 texelSize = vec2(texelWidth, texelHeight);

    float shadow = 0.0;
    
    //Simple 3x3 filter
    for(int x = -1; x <= 1; x++)
    {
        for(int y = -1; y <= 1; y++)
        {
            vec2 offset = vec2(y,x) * texelSize;
            float depth = texture(shadowMap[mapIndex], shadowCoords.xy + offset).x;

            if(depth + bias < shadowCoords.z)
            {
                shadow += 0.0;
            }
            else
            {
                shadow += 1.0;
            }
        }    
    }

    return shadow / 9.0;
}

void main()
{
    
    // properties
    vec3 normal = gl_FrontFacing ? normalize(fs_in.Normal) : -normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 result;

    // phase 1: directional lights

    // phase 2: point lights
    
    for(int i = 0; i < numberOfPointLights; i++)
    {
        result += CalcPointLight(pointLights[i], normal, viewDir);           
    }

    //phase 3: spot light 
    for(int i = 0; i < numberOfSpotLightsFRAG; i++)
    {
        result += CalcSpotLight(spotLights[i], normal, viewDir, i);           
    }

    FragColor = vec4(result, 1.0);   

}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, int lightIndex)
{

    vec3 lightDir = normalize(light.position - fs_in.FragPos);
    vec3 halfDir = normalize(lightDir + viewDir);
    float specAngle = max(dot(halfDir, normal), 0.0);
    float spec = pow(specAngle, 32.0);

    // Calculate the spotlight direction
    vec3 spotDir = normalize(light.direction);

    // Calculate the spotlight factor based on cone angles
    float spotAngle = dot(-lightDir, spotDir);
    float spotFactor = smoothstep(light.outerCutOff, light.cutOff, spotAngle);

    // Calculate the diffuse component
    //When Lambertian coefficient is close to 1, it means that the surface is facing the light source directly
    //When Lambertian coefficient is close to 0, it means that the surface is nearly perpendicular to the direction of the light source 
    float diff = dot(normal, lightDir);  

    //If the light is not directed at fragment, return no color
    if(diff < 0.0f){
        return vec3(0.0f, 0.0f, 0.0f);
    }

    //Calculate shade factor of fragment
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace[lightIndex], light.position, lightIndex);                      

    //Apply colors
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;

    if(hasTexture)
    {
        ambient = vec3(texture(material.diffuse, fs_in.TexCoords)) * spotFactor;
        diffuse = vec3(texture(material.diffuse, fs_in.TexCoords)) * spotFactor;
        specular  = vec3(texture(material.diffuse, fs_in.TexCoords))* spotFactor;
    }
    else
    {
        ambient = object.ambient* spotFactor;
        diffuse = object.diffuse * spotFactor;
        specular = object.specular* spotFactor;
    }

    ambient *= light.ambient;
    diffuse *= light.diffuse * diff ;
    specular *= light.specular * spec ;
        
    // Calculate the attenuation factor (light fall off)
    float distance = length(light.position - fs_in.FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
        
    vec3 lighting = (ambient  + ((shadow) * (diffuse + specular)));

    return lighting;

}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir)
{
    // ambient
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    // calculate diffuse 
    vec3 lightDir = normalize(light.position - fs_in.FragPos);
    float diff = dot(normal, lightDir);
    
    //If the light is not directed at fragment, return no color
    if(diff < 0.0f){
        return vec3(0.0f);
    }


    // calculate specular
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    //Blinn-Phong halfway vector
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    
    //Apply light
    if(hasTexture)
    {
        ambient = texture(material.diffuse, fs_in.TexCoords).rgb;
        diffuse = texture(material.diffuse, fs_in.TexCoords).rgb;  
        specular = texture(material.specular, fs_in.TexCoords).rgb;  
    }
    else
    {
        ambient = object.ambient;
        diffuse = object.diffuse;
        specular = object.specular;
    }

    ambient *= light.ambient;
    diffuse *= light.diffuse * diff ;
    specular *= light.specular * spec ;

    // attenuation
    float distance    = length(light.position - fs_in.FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    ambient  *= attenuation;  
    diffuse   *= attenuation;
    specular *= attenuation;   
        
    vec3 result = ambient + diffuse + specular;

    return result;
}