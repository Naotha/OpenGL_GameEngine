#version 460 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;

out vec2 vertTexCoord;

void main()
{
    vertTexCoord = inTexCoord;
    gl_Position = vec4(inPos, 1.0);
}