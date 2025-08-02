#version 330 core
out float gl_FragDepth;
in vec2 TexCoords;

uniform samplerCube sourceDepthMap; // 输入深度贴图（ping 或 pong）
uniform int faceIndex;

float PreFilterDepth(vec3 sampleDir)
{
    float depth = 0.0;
    int samples = 9;
    vec3 offsets[9] = vec3[](
        vec3(0.0, 0.0, 0.0),
        vec3(0.01, 0.0, 0.0), vec3(-0.01, 0.0, 0.0),
        vec3(0.0, 0.01, 0.0), vec3(0.0, -0.01, 0.0),
        vec3(0.01, 0.01, 0.0), vec3(0.01, -0.01, 0.0),
        vec3(-0.01, 0.01, 0.0), vec3(-0.01, -0.01, 0.0)
    );
    for (int i = 0; i < samples; ++i) {
        depth += texture(sourceDepthMap, sampleDir + 0.005 * offsets[i]).r;
    }
    return depth / float(samples);
}

void main()
{
    float u = TexCoords.x * 2.0 - 1.0;
    float v = TexCoords.y * 2.0 - 1.0;
    vec3 sampleDir;
    if (faceIndex == 0) sampleDir = vec3(1.0, -v, -u);      // +X
    else if (faceIndex == 1) sampleDir = vec3(-1.0, -v, u);  // -X
    else if (faceIndex == 2) sampleDir = vec3(u, 1.0, v);   // +Y
    else if (faceIndex == 3) sampleDir = vec3(u, -1.0, -v);   // -Y
    else if (faceIndex == 4) sampleDir = vec3(u, -v, 1.0);   // +Z
    else if (faceIndex == 5) sampleDir = vec3(-u, -v, -1.0); // -Z
    gl_FragDepth = PreFilterDepth(normalize(sampleDir));
}