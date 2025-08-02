// Fragment Shader
//
// Created by asi on 2024/8/4.

#version 410 core

struct Material
{
    sampler2D diffuse1;
    sampler2D specular1;
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
uniform samplerCube depthMap;

in vec3 fragPos;
in vec3 normal;
in vec2 textCoord;

uniform vec3 viewPos;

out vec4 FragColor;

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

float ShadowCalculation(vec3 fPos, vec3 lPos)
{
    vec3 fragToLight = fPos - lPos;
    float currentDepth = length(fragToLight);
    float shadow = 0.0;
    float bias = 0.05;
    int samples = 20;

    float diskRadius = 0.01;
    for (int i = 0; i < samples; ++i) {
        float closestDepth = texture(depthMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= 100.0;
        shadow += smoothstep(0.0, bias, currentDepth - closestDepth);  // ¡û Ìæ´úÓ²ãÐÖµÅÐ¶Ï
    }
    shadow /= float(samples);


    return shadow;
}

void main()
{
    vec3 ambient = vec3(0.f, 0.f, 0.f);
    vec3 diffuse = vec3(0.f, 0.f, 0.f);
    vec3 specular = vec3(0.f, 0.f, 0.f);

    vec3 n = normalize(normal);
    vec3 viewDir = normalize(viewPos - fragPos);
    // ambient color
    ambient = 0.0f * vec3(texture(material.diffuse1, textCoord));

    // spot light effect
    float shadow = 0.f;
    for (int i = 0; i < num_spot_lights; i++)
    {
        if (spot_lights[i].on)
        //if (false)
        {
            shadow = ShadowCalculation(fragPos, spot_lights[i].position);
            vec3 lightDir = normalize(spot_lights[i].position - fragPos);
            // diffuse color
            float diffusefactor = max(dot(n, lightDir), 0.f);
            vec3 _diffuse = vec3(texture(material.diffuse1, textCoord)) * diffusefactor;
            // specular color
            vec3 halfwayDir = normalize(lightDir + viewDir);
            float specularfactor = pow(max(dot(n, halfwayDir), 0.f), 32);
            vec3 _specular = vec3(texture(material.specular1, textCoord)) * specularfactor;

            float distance = length(spot_lights[i].position - fragPos);
        
            float attenuation = (1 - shadow) *  1.0 / (spot_lights[i].constant + spot_lights[i].linear * distance + spot_lights[i].quadratic * (distance * distance));
            _diffuse *= attenuation * spot_lights[i].color;
            _specular *= attenuation * spot_lights[i].color;
      
            diffuse += _diffuse;
            specular += _specular;
        }
    }

    // blender color
    FragColor = vec4(ambient + diffuse + specular, 1.0f);
}
