// Light Shader
//
// Created by asi on 2024/8/18.

#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTextCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 project;

void main()
{
    gl_Position = project * view * model * vec4(aPosition, 1.0);
}
