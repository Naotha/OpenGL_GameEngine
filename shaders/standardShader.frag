#version 460 core

struct Material
{
    // Diffuse textures
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_diffuse3;
    sampler2D texture_diffuse4;
    sampler2D texture_diffuse5;
    sampler2D texture_diffuse6;
    sampler2D texture_diffuse7;
    sampler2D texture_diffuse8;
    // Specular textures
    sampler2D texture_specular1;
    sampler2D texture_specular2;
    sampler2D texture_specular3;
    sampler2D texture_specular4;
    sampler2D texture_specular5;
    sampler2D texture_specular6;
    sampler2D texture_specular7;
    sampler2D texture_specular8;
    float     shininess;
};

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
uniform Material u_material;

uniform DirectionalLight u_dirLight;
#define POINT_LIGHT_NUM 8
uniform int u_pointLightsNum;
uniform PointLight u_pointLights[POINT_LIGHT_NUM];
#define SPOT_LIGHT_NUM 8
uniform int u_spotLightsNum;
uniform SpotLight u_spotLights[SPOT_LIGHT_NUM];

in vec3 vertNorm;
in vec2 vertTexCoord;
in vec3 vertFragPos;

out vec4 fragCol;

vec3 CalculateDirLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 diffuseMapValues, vec3 specularMapValues);
vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 diffuseMapValues, vec3 specularMapValues);
vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 diffuseMapValues, vec3 specularMapValues);

void main()
{
    // Map values
    vec3 diffuseMapValues = (texture(u_material.texture_diffuse1, vertTexCoord)).rgb;
    vec3 specularMapValues = (texture(u_material.texture_specular1, vertTexCoord)).rgb;
    
    // Light calculations
    vec3 norm = normalize(vertNorm);
    vec3 viewDir = normalize(u_viewPos - vertFragPos);
    // Directional Lighting
    vec3 result = CalculateDirLight(u_dirLight, norm, viewDir, diffuseMapValues, specularMapValues);
    // Point Lights
    for (int i = 0; i < u_pointLightsNum; i++)
        result += CalculatePointLight(u_pointLights[i], norm, viewDir, diffuseMapValues, specularMapValues);
    // Spot Lights
    for (int i = 0; i < u_spotLightsNum; i++)
        result += CalculateSpotLight(u_spotLights[i], norm, viewDir, diffuseMapValues, specularMapValues);

    fragCol = vec4(result, 1.0f);
}

vec3 CalculateDirLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 diffuseMapValues, vec3 specularMapValues)
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
        float spec = pow(clamp(dot(viewDir, reflectDir), 0.0f, 1.0f), u_material.shininess);
        specular = (specularMapValues * spec) * light.specular;
    }

    return (ambient + diffuse + specular);
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 diffuseMapValues, vec3 specularMapValues)
{
    // Ambient
    vec3 ambient = diffuseMapValues * light.ambient;
    // Diffuse
    vec3 lightDir = normalize(light.position - vertFragPos);
    float diff = clamp(dot(normal, lightDir), 0.0f, 1.0f);
    vec3 diffuse = (diff * diffuseMapValues) * light.diffuse;
    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 specular = vec3(0.0f);
    if (diff > 0.0f)
    {
        float spec = pow(clamp(dot(viewDir, reflectDir), 0.0f, 1.0f), u_material.shininess);
        specular = (specularMapValues * spec) * light.specular;
    }
    // Attenuation
    float distance = length(light.position - vertFragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 diffuseMapValues, vec3 specularMapValues)
{   
    // Ambient
    vec3 ambient = diffuseMapValues * light.ambient;
    // Diffuse
    vec3 lightDir = normalize(light.position - vertFragPos);
    float diff = clamp(dot(normal, lightDir), 0.0f, 1.0f);
    vec3 diffuse = (diff * diffuseMapValues) * light.diffuse;
    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 specular = vec3(0.0f);
    if (diff > 0.0f)
    {
        float spec = pow(clamp(dot(viewDir, reflectDir), 0.0f, 1.0f), u_material.shininess);
        specular = (specularMapValues * spec) * light.specular;
    }
    // Attenuation
    float distance = length(light.position - vertFragPos);
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