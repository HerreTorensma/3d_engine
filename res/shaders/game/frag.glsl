#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

in float Visibility;

uniform sampler2D texture1;
uniform vec3 fogColor;

// const int colorLevels = 8;
// const int colorLevels = 32;
const int colorLevels = 16;

const int colors_amount = 16;

const vec3 palette[5] = vec3[5](
    vec3(0.0, 0.0, 0.0),
    vec3(0.25, 0.25, 0.25),
    vec3(0.5, 0.5, 0.5),
    vec3(0.75, 0.75, 0.75),
    vec3(1.0, 1.0, 1.0)
);
const int palette_length = 5;

float rand(vec2 co)
{
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 getNearestPaletteColor(vec3 color) {
    float minDist = distance(color, palette[0]);
    vec3 nearestColor = palette[0];
    
    for (int i = 1; i < palette_length; i++) {
        float dist = distance(color, palette[i]);
        if (dist < minDist) {
            minDist = dist;
            nearestColor = palette[i];
        }
    }
    return nearestColor;
}

// void main()
// {
//     FragColor = texture(texture1, TexCoord);
//     // if (FragColor != vec4(0)) {
//         // FragColor = mix(vec4(fogColor, 1.0), FragColor, Visibility);
//     // }

//     // FragColor.rgb += vec3(rand(TexCoord - 0.5)) * 0.3;

//     // FragColor = vec4(getNearestPaletteColor(FragColor.rgb), FragColor.a);

//     float scale = float(colorLevels - 1);
//     // float scale = float(colorLevels);
//     // vec3 noise = vec3(rand(TexCoord) - 0.5) * 0.3;
//     // vec3 quantizedColor = floor((FragColor.rgb + noise) * scale + 0.5) / scale;
//     vec3 quantizedColor = floor(FragColor.rgb * scale + 0.5) / scale;
//     FragColor = vec4(quantizedColor, FragColor.a);

//     // FragColor = round(FragColor / colors_amount) * colors_amount;
// }

void main()
{
    FragColor = texture(texture1, TexCoord);
}