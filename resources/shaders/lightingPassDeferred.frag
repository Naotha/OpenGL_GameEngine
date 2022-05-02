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

in vec2 vertTexCoord;

out vec4 fragCol;

vec3 CalculateDirLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 diffuseMapValues, float specularMapValue);
vec3 CalculatePointLight(PointLight light, vec3 position, vec3 normal, vec3 viewDir, vec3 diffuseMapValues, float specularMapValue);
vec3 CalculateSpotLight(SpotLight light, vec3 position, vec3 normal, vec3 viewDir, vec3 diffuseMapValues, float specularMapValue);

void main()
{
    // Map values
    vec3 deferredFragPos = texture(gPosition, vertTexCoord).rgb;
    vec3 deferredNormal = texture(gNormal, vertTexCoord).rgb;
    vec3 diffuseMapValues = (texture(gAlbedoSpec, vertTexCoord)).rgb;
    float specularMapValue = (texture(gAlbedoSpec, vertTexCoord)).a;

    // Light calculations
    vec3 norm = deferredNormal;
    vec3 viewDir = normalize(u_viewPos - deferredFragPos);
    // Directional Lighting
    vec3 result = CalculateDirLight(u_dirLight, norm, viewDir, diffuseMapValues, specularMapValue);
    // Point Lights
    for (int i = 0; i < u_pointLightsNum; i++)
    result += CalculatePointLight(u_pointLights[i], deferredFragPos, norm, viewDir, diffuseMapValues, specularMapValue);
    // Spot Lights
    for (int i = 0; i < u_spotLightsNum; i++)
    result += CalculateSpotLight(u_spotLights[i], deferredFragPos, norm, viewDir, diffuseMapValues, specularMapValue);

    //fragCol = vec4(result, 1.0f);
    fragCol = vec4(vec3(vertTexCoord, 1.0f), 1.0f);
}

vec3 CalculateDirLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 diffuseMapValues, float specularMapValue)
{
    // Ambient
    vec3 ambient = diffuseMapValues * light.ambient;
    // Diffuse
    vec3 lightDir = normalize(-light.direction);
    float diff = clamp(dot(normal, lightDir), 0.0f, 1.0f);
    vec3 diffuse = (diff * diffuseMapValues) * light.diffuse;
    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 specular = vec3(0.0f);
    if (diff > 0.0f)
    {
        float spec = pow(clamp(dot(viewDir, reflectDir), 0.0f, 1.0f), 12.0f);
        specular = (specularMapValue * spec) * light.specular;
    }

    return (ambient + diffuse + specular);
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
        float spec = pow(clamp(dot(viewDir, reflectDir), 0.0f, 1.0f), 12.0f);
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
        float spec = pow(clamp(dot(viewDir, reflectDir), 0.0f, 1.0f), 12.0f);
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