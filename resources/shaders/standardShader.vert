#version 460 core

uniform mat4 u_vp;
uniform mat4 u_mvp;
uniform mat4 u_model;
uniform mat4 u_modelIT;

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNorm;
layout (location = 2) in vec2 inTexCoord;

out vec3 vertNorm;
out vec2 vertTexCoord;
out vec3 vertFragPos;

void main()
{
    //gl_Position = u_mvp * u_model * vec4(inPos, 1.0);
    gl_Position = u_vp * u_model * vec4(inPos, 1.0);

    vertNorm = (u_modelIT * vec4(inNorm, 0.0f)).xyz;
    vertTexCoord = inTexCoord;
    vertFragPos = (u_model * vec4(inPos, 1.0f)).xyz;
}