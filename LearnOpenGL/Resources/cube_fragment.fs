// Fragment Shader
//
// Created by asi on 2024/8/4.

#version 330 core
//in vec3 color;
in vec3 fragPos;
in vec3 normal;
in vec2 textCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 objectColor;

out vec4 FragColor;

void main()
{
    //FragColor = vec4(color, 1.0f) * mix(texture(texture1, textCoord), texture(texture2, textCoord), 0.2);
    //FragColor = mix(texture(texture1, textCoord), texture(texture2, textCoord), 0.2);
    //FragColor = texture(texture1, textCoord);

    // light diraction
    vec3 n = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);

    // diffuse color
    float diffuseStrength = max(dot(n, lightDir), 0.f);
    vec3 diffuse = diffuseStrength * lightColor;

    // ambient color
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;
    
    // blender color
    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0f) * texture(texture1, textCoord);
}
