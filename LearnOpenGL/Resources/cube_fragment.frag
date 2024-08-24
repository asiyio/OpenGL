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

struct DirLight
{
    bool on;

    vec3 direction;
    vec3 color;
};

struct SpotLight
{
    bool on;

    vec3 position;
    vec3 color;

    float constant;
    float linear;
    float quadratic;
};

struct FlashLight
{
    bool on;

    vec3 position;
    vec3 direction;
    vec3 color;

    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;
};

uniform DirLight dir_lights[10];
uniform int num_dir_lights;

uniform SpotLight spot_lights[10];
uniform int num_spot_lights;

uniform FlashLight flash_lights[10];
uniform int num_flash_lights;

uniform Material material;

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

    vec3 ambient = vec3(0.f, 0.f, 0.f);
    vec3 diffuse = vec3(0.f, 0.f, 0.f);
    vec3 specular = vec3(0.f, 0.f, 0.f);

    vec3 n = normalize(normal);
    vec3 viewDir = normalize(viewPos - fragPos);
    // ambient color
    ambient = 0.1 * vec3(texture(material.diffuse, textCoord));

    // spot light effect
    for (int i = 0; i < num_spot_lights; i++)
    {
        if (spot_lights[i].on)
        {
            vec3 lightDir = normalize(spot_lights[i].position - fragPos);
            // diffuse color
            float diffusefactor = max(dot(n, lightDir), 0.f);
            vec3 _diffuse = vec3(texture(material.diffuse, textCoord)) * diffusefactor;
            // specular color
            vec3 reflectDir = reflect(-lightDir, n);
            float specularfactor = pow(max(dot(viewDir, reflectDir), 0.f), material.shininess);
            vec3 _specular = vec3(texture(material.specular, textCoord)) * specularfactor;

            float distance = length(spot_lights[i].position - fragPos);
            float attenuation = 1.0 / (spot_lights[i].constant + spot_lights[i].linear * distance + spot_lights[i].quadratic * (distance * distance));
            _diffuse *= attenuation * spot_lights[i].color;
            _specular *= attenuation * spot_lights[i].color;

            diffuse += _diffuse;
            specular += _specular;
        }
    }

    // flash light effect
    for (int i = 0; i < num_flash_lights; i++)
    {
        if (flash_lights[i].on)
        {
            vec3 lightDir = normalize(flash_lights[i].position - fragPos);
            // diffuse color
            float diffusefactor = max(dot(n, lightDir), 0.f);
            vec3 _diffuse = vec3(texture(material.diffuse, textCoord)) * diffusefactor;
            // specular color
            vec3 reflectDir = reflect(-lightDir, n);
            float specularfactor = pow(max(dot(viewDir, reflectDir), 0.f), material.shininess);
            vec3 _specular = vec3(texture(material.specular, textCoord)) * specularfactor;
            
            float theta = dot(lightDir, normalize(-flash_lights[i].direction));
            float epsilon = flash_lights[i].cutOff - flash_lights[i].outerCutOff;
            float intensity = clamp((theta - flash_lights[i].outerCutOff) / epsilon, 0.f, 1.f);
            _diffuse *= intensity;
            _specular *= intensity;

            float distance = length(flash_lights[i].position - fragPos);
            float attenuation = 1.0 / (flash_lights[i].constant + flash_lights[i].linear * distance + flash_lights[i].quadratic * (distance * distance));
            _diffuse *= attenuation * flash_lights[i].color;
            _specular *= attenuation * flash_lights[i].color;

            diffuse += _diffuse;
            specular += _specular; 
        }
    }

    // blender color
    FragColor = vec4(ambient + diffuse + specular, 1.0f);
}
