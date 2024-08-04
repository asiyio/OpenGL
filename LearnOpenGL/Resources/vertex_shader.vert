// Vertex Shader
//
// Created by asi on 2024/8/4.

#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;
out vec3 color;
uniform float time;
void main(){
   vec4 position = vec4(aPosition, 1.0);
   gl_Position = position;
   color = aColor * (sin(time) * 0.5f + 0.5f);
}
