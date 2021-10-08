#version 460 core
struct Material
{
    vec3  ambient;
    vec3  diffuse;
    vec3  specular;
    float shininess;
};

uniform sampler2D u_tex0;
uniform sampler2D u_tex1;
uniform vec3 u_lightCol;
uniform vec3 u_lightPos;
uniform vec3 u_viewPos;
uniform Material material;

in vec3 vertNorm;
in vec2 vertTexCoord;
in vec3 vertFragPos;

out vec4 fragCol;

void main()
{
    // Ambient
    vec3 ambient = material.ambient * u_lightCol;
    // Diffuse
    vec3 norm = normalize(vertNorm);
    vec3 lightDir = normalize(u_lightPos - vertFragPos);
    float diff = clamp(dot(norm, lightDir), 0.0f, 1.0f);
    vec3 diffuse = (diff * material.diffuse) * u_lightCol;
    // Specular
    vec3 viewDir = normalize(u_viewPos - vertFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 specular = vec3(0.0f);
    if (diff > 0.0f)
    {
        float spec = pow(clamp(dot(viewDir, reflectDir), 0.0f, 1.0f), material.shininess);
        specular = (material.specular * spec) * u_lightCol;
    }

    vec3 objectCol = (mix(texture(u_tex0, vertTexCoord), texture(u_tex1, vertTexCoord), 0.4f)).xyz;

    vec3 result = objectCol * (ambient + diffuse + specular);

    fragCol = vec4(result, 1.0f);
}