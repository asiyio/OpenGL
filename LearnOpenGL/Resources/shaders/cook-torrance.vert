// Vertex Shader
//
// Created by asi on 2024/8/4.

#version 410 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTextCoord;

uniform mat4 project;
uniform mat4 view;
uniform mat4 model;

out VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec2 textCoord;
} vs_out;

void main()
{
    vec4 position = vec4(aPosition, 1.0);
    gl_Position = project * view * model * position;

    // send to fragment
    vs_out.fragPos = vec3(model * position);
    vs_out.normal = transpose(inverse(mat3(model))) * aNormal;
    vs_out.textCoord = aTextCoord;
}
