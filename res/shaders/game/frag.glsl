#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

in float Visibility;

uniform sampler2D texture1;
uniform vec3 fogColor;

const int colorLevels = 32;

float rand(vec2 co)
{
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
    FragColor = texture(texture1, TexCoord);
    if (FragColor != vec4(0)) {
        FragColor = mix(vec4(fogColor, 1.0), FragColor, Visibility);
    }

    float scale = float(colorLevels - 1);
    vec3 noise = vec3(rand(TexCoord) - 0.5) * 0.3;
    // vec3 quantizedColor = floor((FragColor.rgb + noise) * scale + 0.5) / scale;
    vec3 quantizedColor = floor(FragColor.rgb * scale + 0.5) / scale;
    FragColor = vec4(quantizedColor, FragColor.a);
}