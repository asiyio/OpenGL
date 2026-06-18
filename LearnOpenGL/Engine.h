//
//  Engine.h
//  LearnOpenGL
//
//  Created by asi on 2024/12/11.
//

#ifndef Engine_h
#define Engine_h

#include <windows.h>
#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <map>
#include <vector>

#include "Light.hpp"
#include "Model.h"
#include "Program.h"

class Engine
{
public:
    void start();

private:
    Engine();
    ~Engine();

    void init();
    bool createWindow();
    void createSceneThings();
    void createVAOs();
    void createFBOs();
    void createTextures();
    void initScene();
    void initImgui();

    // 生成SSAO kernel向量
    float randf();
    void generateSSAOKernel(unsigned int kernelSize = 64);
    // 生成SSAO noise贴图
    void generateSSAONoise(unsigned int noiseSize = 16);

    // 渲染主循环
    void renderLoop();

    // 创建并初始化深度贴图
    void createDepthBuffer();

    // 渲染深度贴图
    void renderDepthBuffer();

    // 渲染场景
    void renderScreen();

    // 渲染imgui菜单
    void renderIngui();

    // 渲染cook-torrance PBR材质
    void renderCookTorrancePBR();

    // 渲染SSAO
    void renderSSAOGBuffer();
    void renderScreenSpaceAmbientOcclusion();
    void renderSSAOBlur();
    void renderSSAOLight();
    void renderQuad();
    void renderSSAOTest();

public:
    static Engine engine;

private:
    GLFWwindow* m_pWindow;

    std::map<std::string, Program*> m_programs;
    std::map<std::string, Model*> m_models;
    std::map<std::string, GLuint> m_VAOs;
    std::map<std::string, GLuint> m_FBOs;
    std::map<std::string, GLuint> m_textures;

    std::vector<PointLight*> m_pointLights;
    std::vector<FlashLight*> m_flashLights;
    std::vector<glm::vec3> m_kernel;

    PointLight* m_pointLight;
    FlashLight* m_flashLight;

    glm::mat4 m_project;
};

#endif /* Engine_h */
