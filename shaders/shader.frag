#version 460 core

uniform sampler2D u_tex0;
uniform sampler2D u_tex1;

in vec3 vertCol;
in vec2 vertTexCoord;

out vec4 fragCol;

void main()
{
    //fragCol = vec4(vertCol, 1.0f);
    fragCol = mix(texture(u_tex0, vertTexCoord), texture(u_tex1, vertTexCoord), 0.4f);
}