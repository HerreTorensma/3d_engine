#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

in float Visibility;

uniform sampler2D texture1;
uniform vec3 fogColor;

void main()
{
    FragColor = texture(texture1, TexCoord);
    if (FragColor != vec4(0)) {
        FragColor = mix(vec4(fogColor, 1.0), FragColor, Visibility);
    }
}