#version 460 core

uniform mat4 u_mvp;

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inCol;
layout (location = 2) in vec2 inTexCoord;

out vec3 vertCol;
out vec2 vertTexCoord;

void main()
{
    gl_Position = u_mvp * vec4(inPos, 1.0);

    vertCol = inCol;
    vertTexCoord = inTexCoord;
}