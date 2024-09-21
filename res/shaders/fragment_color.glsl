#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform vec4 color;

void main()
{
    // FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    FragColor = color;
}