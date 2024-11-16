#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

uniform bool enable_transparency;

void main()
{
    FragColor = texture(texture1, TexCoord);

    // FragColor.a = 0.0625;
    if (enable_transparency) {
        FragColor.a = (1.0 / 8);
    }
}