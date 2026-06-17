#version 330 core
out vec4 FragColor;

const int MAX_NR_POINT_LIGHTS = 8;
const int MAX_NR_SPOT_LIGHTS = 8;
const int MAX_NR_SHADOW_MAPS = 8;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D ssao;

uniform vec3 viewPos;

struct DirLight {
    vec3 direction;	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;
    sampler2D shadowMap;
    bool showShadowArea;
    mat4 directionalLightSpaceMatrix;
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

struct LightingTerms
{
    float diffuse;
    float specular;
};


// Properties sent to fragment shader from vertex shader
/*
in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace[MAX_NR_SHADOW_MAPS];
    vec4 dirFragPosLightSpace;
} fs_in;
*/

//Scene Light Objects
uniform DirLight dirLight;
uniform bool hasDirLight;
uniform PointLight pointLights[MAX_NR_POINT_LIGHTS];
uniform int numberOfPointLights;
uniform SpotLight spotLights[MAX_NR_SPOT_LIGHTS];
uniform int numberOfSpotLights;

// function prototypes
Surface BuildSurface();
LightingTerms ComputeBlinnPhong(vec3 normal, vec3 lightDir, vec3 viewDir, float shininess);

float SpotLightShadowCalculation(vec3 worldPos, SpotLight light);
float DirectionalLightShadowCalculation(float dotLightNormal);
float PointLightShadowCalculation(vec3 fragPos, PointLight light);

vec3 EvaluateSpotLight(Surface surface, vec3 viewDir, SpotLight light, int lightIndex);
vec3 EvaluatePointLight(Surface surface, vec3 viewDir, PointLight light);
vec3 EvaluateDirectionalLight(Surface surface, vec3 viewDir, DirLight light);

Surface BuildSurface()
{
    Surface s;

    vec3 position = texture(gPosition, TexCoords).rgb;
    vec3 normal = normalize(texture(gNormal, TexCoords).rgb);
    vec4 albedoSpec = texture(gAlbedoSpec, TexCoords);

    s.position = position;
    s.normal = normal;
    s.uv = TexCoords;

    s.diffuse = albedoSpec.rgb;

    float specularStrength = albedoSpec.a;
    s.specular = vec3(specularStrength);

    s.ambient = s.diffuse;

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

float SpotLightShadowCalculation(vec3 worldPos, SpotLight light)
{
    vec4 fragPosLightSpace = light.lightSpaceMatrix * vec4(worldPos, 1.0);

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // Outside light projection: no shadow contribution.
    if (projCoords.z > 1.0) return 0.0;
    if (projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0)
    {
        return 0.0;
    }

    float distanceToLight = length(worldPos - light.position);
    if (distanceToLight > light.far_plane) return 0.0;

    float bias = light.bias;

    vec2 texelSize = 1.0 / textureSize(light.shadowMap, 0);

    float shadow = 0.0;
    int n = 2;

    if (distanceToLight > light.far_plane * 0.8)
        n = 1;

    int sampleCount = 0;

    for (int x = -n; x <= n; x++)
    {
        for (int y = -n; y <= n; y++)
        {
            vec2 offset = vec2(x, y) * texelSize;

            float closestDepth = texture(light.shadowMap, projCoords.xy + offset).r;
            float currentDepth = projCoords.z;

            shadow += currentDepth - bias > closestDepth ? 1.0 : 0.0;
            sampleCount++;
        }
    }

    return shadow / float(sampleCount);
}


float DirectionalLightShadowCalculation(float dotLightNormal, Surface surface)
{
    vec4 fragPosLightSpace = dirLight.directionalLightSpaceMatrix * vec4(surface.position, 1.0);

    // Perspective divide.
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // Convert from NDC [-1, 1] to texture coordinates [0, 1].
    projCoords = projCoords * 0.5 + 0.5;

    // unshadow region
    if (projCoords.z > 1.0) return 0.0;

    if (projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0)
    {
        return dirLight.showShadowArea ? 1.0 : 0.0;
    }

    float bias = max(0.05 * (1.0 - dotLightNormal), 0.005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(dirLight.shadowMap, 0);

    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float closestDepth =
                texture(dirLight.shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;

            float currentDepth = projCoords.z;

            shadow += currentDepth - bias > closestDepth ? 1.0 : 0.0;
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
        
    // 1.0 = fully in shadow, 0.0 = fully lit
    return shadow;
}

void main()
{
    Surface surface = BuildSurface();

    vec3 viewDir = normalize(viewPos - surface.position);

    vec3 result = vec3(0.f);

    if (hasDirLight)
        result += EvaluateDirectionalLight(surface, viewDir, dirLight);

    for (int i = 0; i < numberOfSpotLights; i++)
        result += EvaluateSpotLight(surface, viewDir, spotLights[i], i);
  
    for (int i = 0; i < numberOfPointLights; i++)
        result += EvaluatePointLight(surface, viewDir, pointLights[i]);

    FragColor = vec4(result, 1.0);
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

    float AmbientOcclusion = texture(ssao, TexCoords).r;

    vec3 ambient = surface.ambient * light.ambient * AmbientOcclusion;
    vec3 diffuse = surface.diffuse * light.diffuse * diff;
    vec3 specular = surface.specular * light.specular * spec;

    //return attenuation * (ambient + (diffuse + specular));
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
    float shadow = DirectionalLightShadowCalculation(dotLightNormal, surface);

    float AmbientOcclusion = texture(ssao, TexCoords).r;

    vec3 ambient = surface.ambient * light.ambient * AmbientOcclusion;
    vec3 diffuse = surface.diffuse * light.diffuse * terms.diffuse;
    vec3 specular = surface.specular * light.specular * terms.specular;

    //return ambient + (diffuse + specular);
    return ambient + (1.0 - shadow) * (diffuse + specular);
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

    float spotAngle = dot(-lightDir, spotDir);

    float spotFactor = smoothstep(light.outerCutOff, light.cutOff, spotAngle);

    float distance = length(light.position - surface.position);

    float attenuation = 1.0 / (
        light.constant +
        light.linear * distance +
        light.quadratic * distance * distance
    );

    float shadow = SpotLightShadowCalculation(
        surface.position,
        light
    );

    float AmbientOcclusion = texture(ssao, TexCoords).r;

    vec3 ambient = surface.ambient * light.ambient * spotFactor * AmbientOcclusion;

    vec3 diffuse = surface.diffuse * light.diffuse * terms.diffuse * spotFactor;

    vec3 specular = surface.specular * light.specular * terms.specular * spotFactor;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + (1.0 - shadow) * (diffuse + specular);
}
