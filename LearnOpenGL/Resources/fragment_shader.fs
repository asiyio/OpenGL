// Fragment Shader
//
// Created by asi on 2024/8/4.

#version 330 core
in vec3 color;
in vec2 textCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

out vec4 FragColor;

void main(){
    FragColor = vec4(color, 1.0f) * mix(texture(texture1, textCoord), texture(texture2, textCoord), 0.2);
}
