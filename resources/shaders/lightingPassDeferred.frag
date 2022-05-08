#version 460 core

struct DirectionalLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;
    float innerCutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform vec3 u_viewPos;
uniform mat4 u_lightVP;

uniform DirectionalLight u_dirLight;
#define POINT_LIGHT_NUM 8
uniform int u_pointLightsNum;
uniform PointLight u_pointLights[POINT_LIGHT_NUM];
#define SPOT_LIGHT_NUM 8
uniform int u_spotLightsNum;
uniform SpotLight u_spotLights[SPOT_LIGHT_NUM];

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D shadowMap;

in vec2 vertTexCoord;

out vec4 fragCol;

vec3 CalculateDirLight(DirectionalLight light, vec3 position, vec3 normal, vec3 viewDir, vec3 diffuseMapValues, float specularMapValue);
vec3 CalculatePointLight(PointLight light, vec3 position, vec3 normal, vec3 viewDir, vec3 diffuseMapValues, float specularMapValue);
vec3 CalculateSpotLight(SpotLight light, vec3 position, vec3 normal, vec3 viewDir, vec3 diffuseMapValues, float specularMapValue);
float CalculateShadow(vec4 lightSpacePosition, float NdotL);

void main()
{
    // Map values
    vec3 deferredFragPos = texture(gPosition, vertTexCoord).rgb;
    vec3 deferredNormal = texture(gNormal, vertTexCoord).rgb;
    vec3 diffuseMapValues = (texture(gAlbedoSpec, vertTexCoord)).rgb;
    float specularMapValue = (texture(gAlbedoSpec, vertTexCoord)).a;

    // Light calculations
    vec3 norm = normalize(deferredNormal);
    vec3 viewDir = normalize(u_viewPos - deferredFragPos);
    // Directional Lighting
    vec3 result = CalculateDirLight(u_dirLight, deferredFragPos, norm, viewDir, diffuseMapValues, specularMapValue);
    // Point Lights
    for (int i = 0; i < u_pointLightsNum; i++)
        result += CalculatePointLight(u_pointLights[i], deferredFragPos, norm, viewDir, diffuseMapValues, specularMapValue);
    // Spot Lights
    for (int i = 0; i < u_spotLightsNum; i++)
        result += CalculateSpotLight(u_spotLights[i], deferredFragPos, norm, viewDir, diffuseMapValues, specularMapValue);

    fragCol = vec4(result, 1.0f);
}

vec3 CalculateDirLight(DirectionalLight light, vec3 position, vec3 normal, vec3 viewDir, vec3 diffuseMapValues, float specularMapValue)
{
    // Ambient
    vec3 ambient = light.ambient;
    // Diffuse
    vec3 lightDir = normalize(-light.direction);
    float NdotL = clamp(dot(normal, lightDir), 0.0f, 1.0f);
    vec3 diffuse = (NdotL) * light.diffuse;
    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 specular = vec3(0.0f);
    if (NdotL > 0.0f)
    {
        float spec = pow(clamp(dot(viewDir, reflectDir), 0.0f, 1.0f), 32.0f);
        specular = spec * light.specular;
    }
    float shadow = CalculateShadow(u_lightVP * vec4(position, 1.0), NdotL);

    return (ambient + (1.0 - shadow) * (diffuse + specular)) * diffuseMapValues;
}

vec3 CalculatePointLight(PointLight light, vec3 position, vec3 normal, vec3 viewDir, vec3 diffuseMapValues, float specularMapValue)
{
    // Ambient
    vec3 ambient = diffuseMapValues * light.ambient;
    // Diffuse
    vec3 lightDir = normalize(light.position - position);
    float diff = clamp(dot(normal, lightDir), 0.0f, 1.0f);
    vec3 diffuse = (diff * diffuseMapValues) * light.diffuse;
    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 specular = vec3(0.0f);
    if (diff > 0.0f)
    {
        float spec = pow(clamp(dot(viewDir, reflectDir), 0.0f, 1.0f), 32.0f);
        specular = (specularMapValue * spec) * light.specular;
    }
    // Attenuation
    float distance = length(light.position - position);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalculateSpotLight(SpotLight light, vec3 position, vec3 normal, vec3 viewDir, vec3 diffuseMapValues, float specularMapValue)
{
    // Ambient
    vec3 ambient = diffuseMapValues * light.ambient;
    // Diffuse
    vec3 lightDir = normalize(light.position - position);
    float diff = clamp(dot(normal, lightDir), 0.0f, 1.0f);
    vec3 diffuse = (diff * diffuseMapValues) * light.diffuse;
    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 specular = vec3(0.0f);
    if (diff > 0.0f)
    {
        float spec = pow(clamp(dot(viewDir, reflectDir), 0.0f, 1.0f), 32.0f);
        specular = (specularMapValue * spec) * light.specular;
    }
    // Attenuation
    float distance = length(light.position - position);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // Softening
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.innerCutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0f, 1.0f);

    ambient  *= intensity * attenuation;
    diffuse  *= intensity * attenuation;
    specular *= intensity * attenuation;
    return (ambient + diffuse + specular);
}

float CalculateShadow(vec4 lightSpacePosition, float NdotL)
{
    vec3 projCoords = lightSpacePosition.xyz / lightSpacePosition.w;
    projCoords = projCoords * 0.5 + 0.5;
    float lightDepth = texture(shadowMap, projCoords.xy).r;
    if (lightDepth == 0.0)
    {
        return 0.0;
    }
    float currentDepth = projCoords.z;
    //float bias = max(0.001 * (1.0 - NdotL), 0.0001);
    float bias = 0.0005;
    float shadow = (currentDepth - bias) > lightDepth ? 1.0 : 0.0;

    return shadow;
}