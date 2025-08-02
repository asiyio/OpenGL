#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform samplerCube depthMap;
uniform int faceIndex;

void main()
{
    // ��� faceIndex ��Χ
    if (faceIndex < 0 || faceIndex > 5) {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0); // ��ɫ��ʾ����
        return;
    }

    // �� TexCoords �� [0, 1] ӳ�䵽 [-1, 1]
    float u = TexCoords.x * 2.0 - 1.0;
    float v = TexCoords.y * 2.0 - 1.0;

    // ���� faceIndex �����������
    vec3 direction;
    if (faceIndex == 0) direction = vec3(1.0, -v, -u);      // +X
    else if (faceIndex == 1) direction = vec3(-1.0, -v, u);  // -X
    else if (faceIndex == 2) direction = vec3(u, 1.0, -v);   // +Y
    else if (faceIndex == 3) direction = vec3(u, -1.0, v);   // -Y
    else if (faceIndex == 4) direction = vec3(u, -v, 1.0);   // +Z
    else if (faceIndex == 5) direction = vec3(-u, -v, -1.0); // -Z

    // �������ֵ
    float depth = texture(depthMap, normalize(direction)).r;
    if (isnan(depth) || depth < 0.0 || depth > 1.0) {
        FragColor = vec4(0.0, 1.0, 0.0, 1.0); // ��ɫ��ʾ�����쳣
        return;
    }

    // ������ֵ��Ϊ�Ҷ���ɫ
    FragColor = vec4(vec3(depth), 1.0);
}