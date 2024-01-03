#version 330 core
out vec4 FragColor;
const int NR_POINT_LIGHTS = 8;
const int NR_SPOT_LIGHTS = 8;

//Shadow Map
uniform sampler2D shadowMap;

//Material Textures
//uniform sampler2D texture_diffuse1;
//uniform sampler2D texture_specular1;
//uniform sampler2D texture_normal1;
//uniform sampler2D texture_height1;


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
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir);

uniform float textureScale;  // Scaling factor for texture coordinates
vec2 scaledTexCoords;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightPosition)
{
    vec3 lightDirection = normalize(lightPosition - fs_in.FragPosLightSpace.xyz);

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
            float depth = texture(shadowMap, shadowCoords.xy + offset).x;

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
    vec3 normal = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    scaledTexCoords = fs_in.TexCoords * 1;
    

    //TO DO: PUT THESE RESULTS IN IF STATEMENTS DEPENDING ON WHICH LIGHT IS ON

    // phase 1: directional lighting
    vec3 result;// = CalcDirLight(dirLight, normal, viewDir);
    // phase 2: point lights
    for(int i = 0; i < numberOfPointLights; i++)
    {
        //result += CalcPointLight(pointLights[i], normal, fs_in.FragPos, viewDir);           
    }
    //phase 3: spot light
    //for(int i = 0; i < numberOfSpotLights; i++)
    {
    }
                result += CalcSpotLight(spotLights[0], normal, viewDir);           

    FragColor = vec4(result, 1.0);   
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir)
{
    //Diffuse (Blinn Phong)
    vec3 lightDir = normalize(light.position - fs_in.FragPos);
    float lambertian = max(dot(normal, lightDir), 0.0);
    float spec = 0.0;

    if(lambertian > 0.0f)
    {
        vec3 halfDir = normalize(lightDir + viewDir);
        float specAngle = max(dot(halfDir, normal), 0.0);
        spec = pow(specAngle, 32.0);
    }

    // Calculate the spotlight direction
    vec3 spotDir = normalize(light.direction);

    // Calculate the spotlight factor based on cone angles
    float spotAngle = dot(-lightDir, spotDir);
    float spotFactor = smoothstep(light.outerCutOff, light.cutOff, spotAngle);

    // Calculate the diffuse component
    float diff = max(dot(normal, lightDir), 0.0);
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, light.position);                      

    vec3 diffuse;
    vec3 specular;
    vec3 ambient;

    if(hasTexture)
    {
        ambient = vec3(texture(material.diffuse, fs_in.TexCoords));
        diffuse = vec3(texture(material.diffuse, fs_in.TexCoords));
        specular  = vec3(texture(material.diffuse, fs_in.TexCoords));
    }
    else
    {
        ambient = light.ambient * spotFactor;
        diffuse = object.diffuse * spotFactor;
        specular = object.specular;
    }

    diffuse *= light.diffuse * lambertian * spotFactor;
    specular *= light.specular * spec * spotFactor;
        
    // Calculate the attenuation factor
    float distance = length(light.position - fs_in.FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
        
    vec3 lighting = (ambient  + ((shadow) * (diffuse + specular)));

    return lighting;

}
