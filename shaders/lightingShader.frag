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
    vec3 position;
    sampler2D shadowMap;
    bool showShadowArea;
};

struct PointLight {
    vec3 position;  
    float padding;
    float constant;
    float linear;
    float far_plane;
    float quadratic;	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float bias;
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
    float bias;
    mat4 lightSpaceMatrix;
};

struct Surface
{
    vec3 position;
    vec3 normal;
    vec2 uv;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

//Object color properties (used if texture is not available or if you want to change tint of texture)
struct Object
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct LightingTerms
{
    float diffuse;
    float specular;
};


//Properties sent to fragment shader from vertex shader
in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace[MAX_NR_SHADOW_MAPS];
    vec4 dirFragPosLightSpace;
} fs_in;

//Player Position
uniform vec3 viewPos;

//Scene Light Objects
uniform DirLight dirLight;
uniform bool hasDirLight;
uniform PointLight pointLights[MAX_NR_POINT_LIGHTS];
uniform int numberOfPointLights;
uniform SpotLight spotLights[MAX_NR_SPOT_LIGHTS];
uniform int numberOfSpotLights;

//Fragment Properties
uniform Material material;
uniform Object object;
uniform bool hasTexture;

// function prototypes
Surface BuildSurface();
LightingTerms ComputeBlinnPhong(vec3 normal, vec3 lightDir, vec3 viewDir, float shininess);

float SpotLightShadowCalculation(vec4 fragPosLightSpace, SpotLight light);
float DirectionalLightShadowCalculation(float dotLightNormal);
float PointLightShadowCalculation(vec3 fragPos, PointLight light);

vec3 EvaluateSpotLight(Surface surface, vec3 viewDir, SpotLight light, int lightIndex);
vec3 EvaluatePointLight(Surface surface, vec3 viewDir, PointLight light);
vec3 EvaluateDirectionalLight(Surface surface, vec3 viewDir, DirLight light);

Surface BuildSurface()
{
    Surface s;

    s.position = fs_in.FragPos;
    s.normal = normalize(fs_in.Normal);
    s.uv = fs_in.TexCoords;

    if (hasTexture)
    {
        s.diffuse = texture(material.diffuse, fs_in.TexCoords).rgb;
        s.specular = texture(material.specular, fs_in.TexCoords).rgb;
    }
    else
    {
        s.diffuse = object.diffuse;
        s.specular = object.specular;
    }

    s.ambient = s.diffuse * object.ambient;

    s.shininess = 32.0;

    return s;
}

LightingTerms ComputeBlinnPhong(vec3 normal, vec3 lightDir, vec3 viewDir, float shininess)
{
    LightingTerms terms;

    terms.diffuse = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    terms.specular = pow(max(dot(normal, halfwayDir), 0.0), shininess);

    return terms;
}


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
    float bias = light.bias; 

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

float DirectionalLightShadowCalculation(float dotLightNormal)
{
    // perform perspective divide
    vec3 pos = fs_in.dirFragPosLightSpace.xyz * 0.5 + 0.5;

    // If the fragment is outside the shadow map bounds, make it fully shadowed
    //Enable this to show directional shadow map boundary
    if(dirLight.showShadowArea)
    {
        if (pos.x < 0.0 || pos.x > 1.0 || pos.y < 0.0 || pos.y > 1.0)
        {
        return 1.0; // Full shadow
        }
    }

    //Fixes cut-off for scene behind light direction (0.0f indicates no shadow, 1.0f indicates full shadow)
    if(pos.z > 1.0f){
        pos.z = 1.0f;
    }


    float depth = texture(dirLight.shadowMap, pos.xy).r;

    //For shadow acne
    float bias = max(0.05 * (1.0 - dotLightNormal), 0.005);

    //PCF (For blurry edges)
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(dirLight.shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float depth = texture(dirLight.shadowMap, pos.xy + vec2(x, y) * texelSize).r;
            shadow += (depth + bias) < pos.z ? 1.0: 0.0;
        }

    }
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

    // PCF (Percentage Closer Filtering) shadow calculation
    float shadow = 0.0;        // Initialize shadow factor
    float bias = light.bias;          // Bias to prevent shadow acne

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
                closestDepth *= light.far_plane;

                // Compare the fragment's depth to the sampled depth
                if(currentDepth - bias > closestDepth)
                    shadow += 1.0;  // Increase shadow factor if in shadow
            }
        }
    }

    // Average out the shadow factor based on the number of samples
    shadow /= (samples * samples * samples);

    // Optionally, display closestDepth for debugging purposes
    // FragColor = vec4(vec3(closestDepth / light.far_plane), 1.0);    
        
    // 1.0 = fully in shadow, 0.0 = fully lit
    return shadow;
}

void main()
{
    Surface surface = BuildSurface();
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);

    vec3 result = vec3(0.f);

    if (hasDirLight)
        result += EvaluateDirectionalLight(surface, viewDir, dirLight);

    for (int i = 0; i < numberOfPointLights; i++)
        result += EvaluatePointLight(surface, viewDir, pointLights[i]);

    for (int i = 0; i < numberOfSpotLights; i++)
        result += EvaluateSpotLight(surface, viewDir, spotLights[i], i);

    FragColor = vec4(result, 1.0);
}

vec3 EvaluateSpotLight(Surface surface, vec3 viewDir, SpotLight light, int lightIndex)
{
    vec3 lightDir = normalize(light.position - surface.position);
    vec3 spotDir = normalize(light.direction);

    LightingTerms terms = ComputeBlinnPhong(
        surface.normal,
        lightDir,
        viewDir,
        surface.shininess
    );

    // Spotlight cone falloff
    float spotAngle = dot(-lightDir, spotDir);
    float spotFactor = smoothstep(light.outerCutOff, light.cutOff, spotAngle);

    // Distance attenuation
    float distance = length(light.position - surface.position);
    float attenuation = 1.0 / (
        light.constant +
        light.linear * distance +
        light.quadratic * distance * distance
    );

    float shadow = SpotLightShadowCalculation(
        fs_in.FragPosLightSpace[lightIndex],
        light
    );

    vec3 ambient = surface.ambient * light.ambient;
    vec3 diffuse = surface.diffuse * light.diffuse * terms.diffuse * spotFactor;
    vec3 specular = surface.specular * light.specular * terms.specular * spotFactor;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + (1.0 - shadow) * (diffuse + specular);
}

// Calculates the color when using a point light.
vec3 EvaluatePointLight(Surface surface, vec3 viewDir, PointLight light)
{
    vec3 lightDir = normalize(light.position - surface.position);

    float diff = max(dot(surface.normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(surface.normal, halfwayDir), 0.0), surface.shininess);

    float distance = length(light.position - surface.position);
    float attenuation = 1.0 / (
        light.constant +
        light.linear * distance +
        light.quadratic * distance * distance
    );

    float shadow = PointLightShadowCalculation(surface.position, light);

    vec3 ambient = surface.ambient * light.ambient;
    vec3 diffuse = surface.diffuse * light.diffuse * diff;
    vec3 specular = surface.specular * light.specular * spec;

    return attenuation * (ambient + (1.0 - shadow) * (diffuse + specular));
}

vec3 EvaluateDirectionalLight(Surface surface, vec3 viewDir, DirLight light)
{
    vec3 lightDir = normalize(light.direction);

    LightingTerms terms = ComputeBlinnPhong(
        surface.normal,
        lightDir,
        viewDir,
        surface.shininess
    );

    float dotLightNormal = dot(lightDir, surface.normal);
    float shadow = DirectionalLightShadowCalculation(dotLightNormal);

    vec3 ambient = surface.ambient * light.ambient;
    vec3 diffuse = surface.diffuse * light.diffuse * terms.diffuse;
    vec3 specular = surface.specular * light.specular * terms.specular;

    return ambient + (1.0 - shadow) * (diffuse + specular);
}
