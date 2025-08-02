// Outlining Effect Shader
//
// Created by asi on 2025/2/12.

#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTextCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 project;

void main()
{
    vec3 Normal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = project * view * (model * vec4(aPosition, 1.0) + vec4(0.02 * Normal, 0));
}

