#version 460 core

in vec3 vertCol;
in vec2 vertTexCoord;

out vec4 fragCol;

uniform sampler2D tex1;
uniform sampler2D tex2;

void main()
{
    //fragCol = vec4(vertCol, 1.0f);
    //fragCol = texture(tex2, vertTexCoord);
    fragCol = mix(texture(tex1, vertTexCoord), texture(tex2, vertTexCoord), 0.4f);
}