#version 460 core

uniform sampler2D u_tex0;
uniform sampler2D u_tex1;

in vec3 vertCol;
in vec2 vertTexCoord;
in vec3 vertFragPos;

out vec4 fragCol;

void main()
{
    vec4 objectCol = mix(texture(u_tex0, vertTexCoord), texture(u_tex1, vertTexCoord), 0.4f);
    fragCol = objectCol;
}