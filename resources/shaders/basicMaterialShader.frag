#version 460 core
struct Material
{
    vec3  ambient;
    vec3  diffuse;
    vec3  specular;
    float shininess;
};

struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 u_viewPos;
uniform Material u_material;
uniform Light u_light;

in vec3 vertNorm;
in vec3 vertFragPos;

out vec4 fragCol;

void main()
{
    // Ambient
    vec3 ambient = u_material.ambient * u_light.ambient;
    // Diffuse
    vec3 norm = normalize(vertNorm);
    vec3 lightDir = normalize(u_light.position - vertFragPos);
    float diff = clamp(dot(norm, lightDir), 0.0f, 1.0f);
    vec3 diffuse = (diff * u_material.diffuse) * u_light.diffuse;
    // Specular
    vec3 viewDir = normalize(u_viewPos - vertFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 specular = vec3(0.0f);
    if (diff > 0.0f)
    {
        float spec = pow(clamp(dot(viewDir, reflectDir), 0.0f, 1.0f), u_material.shininess * 128);
        specular = (u_material.specular * spec) * u_light.specular;
    }

    vec3 result = ambient + diffuse + specular;

    fragCol = vec4(result, 1.0f);
}