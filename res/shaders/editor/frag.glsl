#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, TexCoord);

    // FragColor.a = 0.0625;
    FragColor.a = (1.0 / 8);
}