#version 460 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inCol;
layout (location = 2) in vec2 inTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 vertCol;
out vec2 vertTexCoord;

void main()
{
    gl_Position = projection * view * model * vec4(inPos, 1.0);

    vertCol = inCol;
    vertTexCoord = inTexCoord;
}