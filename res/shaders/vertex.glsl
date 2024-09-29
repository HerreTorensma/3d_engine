#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

out float Visibility;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

const float fogDensity = 0.2;
const float fogGradient = 3.0;

void main()
{
    vec4 worldPosition = model * vec4(aPos, 1.0);
    vec4 posRelativeToCam = view * worldPosition;

    gl_Position = projection * posRelativeToCam;
    TexCoord = aTexCoord;

    float distance = length(posRelativeToCam.xyz);
    Visibility = exp(-pow((distance * fogDensity), fogGradient));
    Visibility = clamp(Visibility, 0.0, 1.0);
}