#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoord;

flat out vec3 FaceNormal;

out float Visibility;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform bool billboard;

// const float fogDensity = 0.2;
const float fogDensity = 0.1;
// const float fogGradient = 3.0;
const float fogGradient = 1.0;

void main()
{
    vec4 worldPosition = model * vec4(aPos, 1.0);
    vec4 posRelativeToCam = view * worldPosition;

    gl_Position = projection * posRelativeToCam;
    TexCoord = aTexCoord;

    float distance = length(posRelativeToCam.xyz);
    Visibility = exp(-pow((distance * fogDensity), fogGradient));
    Visibility = clamp(Visibility, 0.0, 1.0);
    // Visibility = clamp(Visibility, 0.05, 1.0);

    // FragNormal = mat3(transpose(inverse(model)));
    FaceNormal = mat3(transpose(inverse(model))) * aNormal;
}