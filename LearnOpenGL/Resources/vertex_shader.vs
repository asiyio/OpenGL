// Vertex Shader
//
// Created by asi on 2024/8/4.

#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTextCoord;

uniform float time;

out vec3 color;
out vec2 textCoord;

void main(){
    vec4 position = vec4(aPosition, 1.0);
    gl_Position = position;
    
    //output color and texture coordinate
    color = aColor * (sin(time) * 0.5f + 0.5f);
    textCoord = aTextCoord;
}
