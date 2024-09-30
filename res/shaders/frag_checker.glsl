#version 330 core

in vec2 TexCoord;  // The texture coordinates passed from the vertex shader
in float Visibility;

out vec4 FragColor; // The output fragment color

uniform sampler2D texture1; // The input texture

uniform vec3 fogColor;


// uniform int colorLevels;    // Number of color levels to quantize
const int colorLevels = 16;

// Checkerboard dithering function
float getCheckerboardDitherValue(vec2 uv)
{
    // Alternate between 0 and 1 based on the pixel's position in the grid
    return mod(floor(uv.x * 2.0) + floor(uv.y * 2.0), 2.0);
}

void main()
{
    // Get the original color from the texture
    vec4 color = texture(texture1, TexCoord);
    if (color != vec4(0)) {
        color = mix(vec4(fogColor, 1.0), color, Visibility);
    }
    
    // Number of discrete levels we want to quantize the color to
    float scale = float(colorLevels - 1);

    // Get the checkerboard dithering value for this pixel
    float ditherValue = getCheckerboardDitherValue(TexCoord) / scale;

    // Add dithering before quantization
    vec3 quantizedColor = floor((color.rgb + ditherValue) * scale + 0.5) / scale;

    // Output the quantized color
    FragColor = vec4(quantizedColor, color.a); // Keep alpha unchanged
}
