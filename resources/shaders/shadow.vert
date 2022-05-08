#version 460 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;

uniform mat4 u_lightVP;
uniform mat4 u_model;

void main()
{
    gl_Position = u_lightVP * u_model * vec4(inPos, 1.0);
}