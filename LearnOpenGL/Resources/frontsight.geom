// Light Shader
//
// Created by asi on 2024/12/8.
#version 330 core

layout(points) in;
layout(line_strip, max_vertices = 2) out;

const float radius = 5;
const int segments = 32;

void main() {
    vec4 center = gl_in[0].gl_Position;

    for (int i = 0; i <= segments; ++i) {
        gl_Position = center + vec4(i, i, 0.0, 0.0);
        EmitVertex();
    }
    EndPrimitive();
}