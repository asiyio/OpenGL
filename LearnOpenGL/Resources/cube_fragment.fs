// Fragment Shader
//
// Created by asi on 2024/8/4.

#version 330 core

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;
};

uniform Material material;
uniform Light light;

in vec3 fragPos;
in vec3 normal;
in vec2 textCoord;

uniform vec3 viewPos;

out vec4 FragColor;

void main()
{
    //FragColor = vec4(color, 1.0f) * mix(texture(texture1, textCoord), texture(texture2, textCoord), 0.2);
    //FragColor = mix(texture(texture1, textCoord), texture(texture2, textCoord), 0.2);
    //FragColor = texture(texture1, textCoord);

    // light direction
    vec3 n = normalize(normal);
    vec3 lightDir = normalize(light.position - fragPos);

    // view direction
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, n);

    // diffuse color
    float diffusefactor = max(dot(n, lightDir), 0.f);
    vec3 diffuse = vec3(texture(material.diffuse, textCoord)) * diffusefactor * light.diffuse;

    // ambient color
    vec3 ambient = vec3(texture(material.diffuse, textCoord)) * light.ambient;

    // specular color
    float specularfactor = pow(max(dot(viewDir, reflectDir), 0.f), material.shininess);
    vec3 specular = vec3(texture(material.specular, textCoord)) * specularfactor * light.specular;
    
    // blender color
    FragColor = vec4(ambient + diffuse + specular, 1.0f);
}
