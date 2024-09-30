#version 330 core

in vec2 TexCoord;  // The texture coordinates passed from the vertex shader
in float Visibility;

out vec4 FragColor; // The output fragment color

uniform sampler2D texture1; // The input texture

uniform vec3 fogColor;


// uniform int colorLevels;    // Number of color levels to quantize
const int colorLevels = 16;

// 4x4 Bayer matrix (flattened into a 1D array and normalized to [0, 1])
float bayerMatrix4x4[16] = float[](
    0.0 / 16.0,  8.0 / 16.0,  2.0 / 16.0, 10.0 / 16.0,
   12.0 / 16.0,  4.0 / 16.0, 14.0 / 16.0,  6.0 / 16.0,
    3.0 / 16.0, 11.0 / 16.0,  1.0 / 16.0,  9.0 / 16.0,
   15.0 / 16.0,  7.0 / 16.0, 13.0 / 16.0,  5.0 / 16.0
);

// Function to get the Bayer matrix value based on pixel coordinates
float getBayerDitherValue(vec2 uv)
{
    // Convert the texture coordinates to a 4x4 grid
    int x = int(mod(floor(uv.x * 4.0), 4.0));
    int y = int(mod(floor(uv.y * 4.0), 4.0));
    
    // Compute the 1D array index for the Bayer matrix
    int index = y * 4 + x; // Convert 2D (x, y) to 1D index
    return bayerMatrix4x4[index];
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

    // Get the Bayer matrix dithering value for this pixel
    float ditherValue = getBayerDitherValue(TexCoord);

    // Add dithering before quantization
    vec3 quantizedColor = floor((color.rgb + ditherValue / scale) * scale + 0.5) / scale;

    // Output the quantized color
    FragColor = vec4(quantizedColor, color.a); // Keep alpha unchanged
}
