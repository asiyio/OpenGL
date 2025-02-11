// Skybox Fragment
//
// Created by asi on 2024/11/4.

#version 410 core

in vec3 textCoord;

uniform samplerCube skybox;

out vec4 FragColor;

void main()
{
    FragColor = texture(skybox, textCoord);
}
