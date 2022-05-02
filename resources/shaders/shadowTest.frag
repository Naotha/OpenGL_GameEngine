#version 460 core
out vec4 fragColor;

in vec2 vertTexCoord;

uniform sampler2D depthMap;

void main()
{
    float depthValue = texture(depthMap, vertTexCoord).r;
    fragColor = vec4(vec3(depthValue), 1.0);
}