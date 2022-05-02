#version 460 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 vertNorm;
in vec2 vertTexCoord;
in vec3 vertFragPos;

struct Material
{
// Diffuse textures
    sampler2D texture_diffuse1;
// Specular textures
    sampler2D texture_specular1;
    float     shininess;
};

uniform Material u_material;

void main()
{
    gPosition = vec4(vertFragPos, 1.0f);
    gNormal = vec4((normalize(vertNorm)), 0.0f);
    gAlbedoSpec.rgb = texture(u_material.texture_diffuse1, vertTexCoord).rgb;
    gAlbedoSpec.a = 1.0f;
}