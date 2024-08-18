// Vertex Shader
//
// Created by asi on 2024/8/4.

#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTextCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 project;

//out vec3 color;
out vec3 fragPos;
out vec3 normal;
out vec2 textCoord;

void main()
{
    vec4 position = vec4(aPosition, 1.0);
    gl_Position = project * view * model * position;

    // send to fragment
    fragPos = vec3(model * position);
    normal = normalize(mat3(transpose(inverse(model))) * aNormal);
    textCoord = aTextCoord;
}
