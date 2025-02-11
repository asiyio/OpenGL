// Skybox Shader
//
// Created by asi on 2024/11/4.

#version 410 core
layout(location = 0) in vec3 aPosition;

uniform mat4 view;
uniform mat4 project;

out vec3 textCoord;

void main()
{
    vec4 pos = project * mat4(mat3(view)) * vec4(aPosition, 1.0);
    textCoord = aPosition;
    gl_Position = pos.xyww;
}
