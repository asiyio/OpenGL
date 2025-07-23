#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform samplerCube depthMap;
uniform int faceIndex;

void main()
{
    // 检查 faceIndex 范围
    if (faceIndex < 0 || faceIndex > 5) {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0); // 红色表示错误
        return;
    }

    // 将 TexCoords 从 [0, 1] 映射到 [-1, 1]
    float u = TexCoords.x * 2.0 - 1.0;
    float v = TexCoords.y * 2.0 - 1.0;

    // 根据 faceIndex 计算采样方向
    vec3 direction;
    if (faceIndex == 0) direction = vec3(1.0, -v, -u);      // +X
    else if (faceIndex == 1) direction = vec3(-1.0, -v, u);  // -X
    else if (faceIndex == 2) direction = vec3(u, 1.0, -v);   // +Y
    else if (faceIndex == 3) direction = vec3(u, -1.0, v);   // -Y
    else if (faceIndex == 4) direction = vec3(u, -v, 1.0);   // +Z
    else if (faceIndex == 5) direction = vec3(-u, -v, -1.0); // -Z

    // 采样深度值
    float depth = texture(depthMap, normalize(direction)).r;
    if (isnan(depth) || depth < 0.0 || depth > 1.0) {
        FragColor = vec4(0.0, 1.0, 0.0, 1.0); // 绿色表示采样异常
        return;
    }

    // 输出深度值作为灰度颜色
    FragColor = vec4(vec3(depth), 1.0);
}