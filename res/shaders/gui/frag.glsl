#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

uniform vec4 color1;

void main() {
    FragColor = color1 * texture(texture1, TexCoord);
}