#version 330 core
out vec4 FragColor;

const int MAX_NR_POINT_LIGHTS = 8;
const int MAX_NR_SPOT_LIGHTS = 8;
const int MAX_NR_SHADOW_MAPS = 8;

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
    samplerCube shadowMap;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;  
    float constant;
    float linear;
    float far_plane;
    float quadratic;  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
    sampler2D shadowMap;
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


uniform float far_plane;


//Player Position
uniform vec3 viewPos;

//Scene Light Objects
uniform DirLight dirLight;
uniform PointLight pointLights[MAX_NR_POINT_LIGHTS];
uniform int numberOfPointLightsFRAG;
uniform SpotLight spotLights[MAX_NR_SPOT_LIGHTS];
uniform int numberOfSpotLightsFRAG;

//Fragment Properties
uniform Material material;
uniform Object object;
uniform bool hasTexture;

// function prototypes
vec3 CalcDirLight(vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, int lightIndex);

float SpotLightShadowCalculation(vec4 fragPosLightSpace, SpotLight light)
{
    
    // Calculate the direction from the fragment to the light position
    vec3 lightDirection = normalize(light.position - fragPosLightSpace.xyz);

    //Get fragment to light distance
    vec3 fragToLight = fragPosLightSpace.xyz - light.position;
    float lightToFragmentDistance = length(fragToLight); // Distance from light to fragment

    if (lightToFragmentDistance > light.far_plane) {
        return 1.0; // Fragment is outside the light's range, fully lit, no shadow
    }

    // Convert fragment position from light space to normalized device coordinates
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // Map coordinates from [-1,1] to [0,1] range
    vec3 shadowCoords = projCoords * 0.5 + vec3(0.5);

    // Calculate the diffuse factor (not directly used in shadow mapping but helpful for understanding)
    float diffuseFactor = dot(fs_in.Normal, -lightDirection);

    // Bias to prevent shadow acne (adjust based on your scene)
    float bias = 0.005; 

    // Calculate the size of a single texel in shadow map texture coordinates
    vec2 texelSize = 1.0 / textureSize(light.shadowMap, 0);

    float shadow = 0.0;

    // Simple 3x3 PCF (Percentage Closer Filtering) filter
    int n = 2; // 1 = 3x3, 2 = 5x5, 3 = 7x7, etc.

    // Adjust PCF filter size based on distance from the light
    if (lightToFragmentDistance > light.far_plane * 0.8) {
        n = 1; // Use smaller filter size for distant fragments
    }

    for(int x = -n; x <= n; x++)
    {
        for(int y = -n; y <= n; y++)
        {
            // Offset for sampling
            vec2 offset = vec2(x, y) * texelSize;

            // Sample the shadow map texture at the offset position
            float depth = texture(light.shadowMap, shadowCoords.xy + offset).r;

            // Compare the sampled depth to the current fragment's depth
            if(depth + bias < shadowCoords.z)
            {
                shadow += 1.0; // In shadow
            }
            else
            {
                shadow += 0.0; // Not in shadow
            }
        }
    }

    // Normalize shadow value by the number of samples (9 for a 3x3 filter)
    // 0.0 = not in shadow, 1.0 = in shadow
    return shadow / 9.0;
}


// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float PointLightShadowCalculation(vec3 fragPos, PointLight light)
{

    // Compute the vector from the fragment position to the light source
    vec3 fragToLight = fragPos - light.position;

    // Calculate the current depth from the light's perspective
    float currentDepth = length(fragToLight);

    // Early exit if the fragment is outside the light's range
    if (currentDepth > far_plane)
    {
        return 1.0; // Fragment is fully shadowed (or outside light range)
    }

    // PCF (Percentage Closer Filtering) shadow calculation
    float shadow = 0.0;        // Initialize shadow factor
    float bias = 0.5;          // Bias to prevent shadow acne

    //Less samples is more blocky but faster, more samples is smoother but slower [4, 16]
    float samples = 4.0;       // Number of samples for PCF

    //Smaller offset is sharper, larger offset is softer [-0.05, 0.2]
    float offset = 0.1;        // Offset for sampling around the fragment


    // Loop through a 3D grid of samples around the fragment's shadow position
    for(float x = -offset; x < offset; x += offset / (samples * 0.5))
    {
        for(float y = -offset; y < offset; y += offset / (samples * 0.5))
        {
            for(float z = -offset; z < offset; z += offset / (samples * 0.5))
            {
                // Sample the shadow map at the offset position
                float closestDepth = texture(light.shadowMap, fragToLight + vec3(x, y, z)).r;

                // Convert the depth from [0,1] range back to the original depth range
                closestDepth *= far_plane;

                // Compare the fragment's depth to the sampled depth
                if(currentDepth - bias > closestDepth)
                    shadow += 1.0;  // Increase shadow factor if in shadow
            }
        }
    }

    // Average out the shadow factor based on the number of samples
    shadow /= (samples * samples * samples);

    // Optionally, display closestDepth for debugging purposes
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
        
    // 1.0 = fully in shadow, 0.0 = fully lit
    return shadow;
}

void main()
{
    
    // properties
    vec3 normal = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 result;

    // phase 1: directional lights
    result += CalcDirLight(normal, viewDir);
    // phase 2: point lights
    
    for(int i = 0; i < numberOfPointLightsFRAG; i++)
    {
        result += CalcPointLight(pointLights[i], normal, viewDir);           
    }

    //phase 3: spot light 
    for(int i = 0; i < numberOfSpotLightsFRAG; i++)
    {
        result += CalcSpotLight(spotLights[i], normal, viewDir, i);           
    }


    FragColor = vec4(result, 1.0);   
   
    //FragColor = texture(material.diffuse, fs_in.TexCoords);
}

// Calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, int lightIndex)
{
    // Calculate direction vectors
    vec3 lightDir = normalize(light.position - fs_in.FragPos); // Direction to light
    vec3 spotDir = normalize(light.direction);                 // Spotlight direction

    // Calculate diffuse (Lambertian reflection)
    float diff = max(dot(normal, lightDir), 0.0);

    // Calculate Blinn-Phong specular
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    // Calculate spotlight factor (cone angles)
    float spotAngle = dot(-lightDir, spotDir);
    float spotFactor = smoothstep(light.outerCutOff, light.cutOff, spotAngle);

    // Calculate shadow factor
    float shadow = SpotLightShadowCalculation(fs_in.FragPosLightSpace[lightIndex], light);

    // Initialize color components
    vec3 ambient, diffuse, specular;

    // Apply texture or object colors
    if (hasTexture)
    {
        //ambient = texture(material.diffuse, fs_in.TexCoords).rgb;
        ambient = object.ambient;
        diffuse = texture(material.diffuse, fs_in.TexCoords).rgb;
        specular = texture(material.specular, fs_in.TexCoords).rgb;
    }
    else
    {
        ambient = object.ambient;
        diffuse = object.diffuse;
        specular = object.specular;
    }

    // Apply lighting influence
    ambient *= light.ambient;
    diffuse *= light.diffuse * diff * spotFactor;
    specular *= light.specular * spec * spotFactor;

    // Calculate attenuation (light fall-off)
    float distance = length(light.position - fs_in.FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Apply attenuation to colors
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    // Combine lighting components
    vec3 result = ambient + (1.0 - shadow) * (diffuse + specular);

    return result;
}

// Calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir)
{
    // Calculate direction to light
    vec3 lightDir = normalize(light.position - fs_in.FragPos);

    // Calculate diffuse component (Lambertian reflection)
    float diff = max(dot(lightDir, normal), 0.0);

    // Calculate specular component (Blinn-Phong reflection)
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    // Initialize color components
    vec3 ambient, diffuse, specular;

    // Apply texture or object colors
    if (hasTexture)
    {
        ambient = object.ambient;
        diffuse = texture(material.diffuse, fs_in.TexCoords).rgb;
        specular = texture(material.specular, fs_in.TexCoords).rgb;
    }
    else
    {
        ambient = object.ambient;
        diffuse = object.diffuse;
        specular = object.specular;
    }

    // Apply light intensity
    ambient *= light.ambient;
    diffuse *= light.diffuse * diff;
    specular *= light.specular * spec;

    // Calculate attenuation (light fall-off)
    float distance = length(light.position - fs_in.FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Apply attenuation to colors
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    // Calculate shadow factor
    float shadow = PointLightShadowCalculation(fs_in.FragPos, light);

    // Combine lighting components with shadow factor
    vec3 result = ambient + (1.0 - shadow) * (diffuse + specular);

    return result;
}

vec3 CalcDirLight(vec3 normal, vec3 viewDir){
    return vec3(0.0f);
}
