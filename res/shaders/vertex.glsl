#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

out float Visibility;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform bool billboard;

const float fogDensity = 0.2;
const float fogGradient = 3.0;

void main()
{
    // if (billboard) {
    //     vec3 cameraPos = vec3(-view[3][0], -view[3][1], -view[3][2]);
    //     vec3 toCamera = normalize(cameraPos - model.xyz);
    //     toCamera.y = 0.0;
    //     vec3 cameraRight = normalize(cross(vec3(0.0, 1.0, 0.0), toCamera));
    //     vec3 cameraUp = vec3(0.0, 1.0, 0.0);
    //     vec3 vertexPos = model.xyz + cameraRight * aPos.x * 64 + cameraUp * aPos.y * 64;

    //     gl_Position = projection * view * vec4(vertexPos, 1.0);
    // } else {
    //     vec4 worldPosition = model * vec4(aPos, 1.0);
    //     vec4 posRelativeToCam = view * worldPosition;

    //     gl_Position = projection * posRelativeToCam;

    //     float distance = length(posRelativeToCam.xyz);
    //     Visibility = exp(-pow((distance * fogDensity), fogGradient));
    //     Visibility = clamp(Visibility, 0.0, 1.0);
    // }
    // // vec3 vertexPos = model.xyz + cameraRight * aPos.x * 64 + cameraUp * aPos.y * 64;
    // // model = vec4(model.xyz + vec3(cameraRight * aPos.x * 64 + cameraUp * aPos.y * 64);

    // TexCoord = aTexCoord;

    vec4 worldPosition = model * vec4(aPos, 1.0);
    vec4 posRelativeToCam = view * worldPosition;

    gl_Position = projection * posRelativeToCam;
    TexCoord = aTexCoord;

    float distance = length(posRelativeToCam.xyz);
    Visibility = exp(-pow((distance * fogDensity), fogGradient));
    Visibility = clamp(Visibility, 0.0, 1.0);
}