// Fragment Shader
//
// Created by asi on 2025/7/29.
// Cook-Torrance PBR (no IBL, single light)

#version 410 core
out vec4 FragColor;
const float PI = 3.14159265359;

// vertex输入
in VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec2 textCoord;
} fs_in;

// 材质
struct Material
{
    sampler2D diffuse;
    sampler2D height;
    sampler2D shininess;
};
uniform Material material;

// 点光源
struct PointLight
{
    vec3 position;
    vec3 color;
    float intensity;
};
uniform PointLight pointLight;

// 动态调整变量
uniform vec3 viewPos;       // 摄像机视角位置
uniform vec3 albedo;        // 基础颜色
uniform float metallic;     // 金属度 (0.0 ~ 1.0)
uniform float roughness;    // 粗糙度 (0.0 ~ 1.0)
uniform float ao;           // 环境遮蔽 (0.0 ~ 1.0)
                            // 0.0：完全遮蔽，几乎没有环境光（很暗）
                            // 1.0：完全暴露，完全接收到环境光（最亮）


vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    // $F = F_0 + (1 - F_0) \cdot (1 - \cos\theta)^5$
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = NdotH2 * (a2 - 1.0) + 1.0;
    return a2 / (PI * denom * denom);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

void main()
{
    vec3 N = normalize(fs_in.normal);
    vec3 V = normalize(viewPos - fs_in.fragPos);
    vec3 L = normalize(pointLight.position - fs_in.fragPos);
    vec3 H = normalize(V + L);

    float distance = length(pointLight.position - fs_in.fragPos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = pointLight.color * pointLight.intensity * attenuation;

    // Fresnel
    vec3 F0 = vec3(0.04); // 非金属默认反射率
    F0 = mix(F0, albedo, metallic);
    vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    // Cook-Torrance terms
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 numerator = NDF * G * F;
    float denominator = max(4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0), 0.001);
    vec3 specular = numerator / denominator;

    // Diffuse term
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    float NdotL = max(dot(N, L), 0.0);
    vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;

    // Ambient (no IBL)
    vec3 ambient = vec3(0.03) * albedo * ao;

    vec3 color = ambient + Lo;

    // HDR tonemapping + gamma correction
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2)); // Gamma correction

    FragColor = vec4(color, 1.0);
}
