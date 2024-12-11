// Light Shader
//
// Created by asi on 2024/12/8.

#version 330 core
layout(location = 0) in vec3 aPosition;

void main()
{
    gl_Position = vec4(aPosition.xy, 0.f, 1.f);
}
