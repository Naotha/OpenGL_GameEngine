#version 460 core

uniform sampler2D u_tex0;
uniform sampler2D u_tex1;
uniform vec3 u_lightCol;

in vec3 vertCol;
in vec2 vertTexCoord;

out vec4 fragCol;

void main()
{
    fragCol = vec4(u_lightCol, 1.0f);
}