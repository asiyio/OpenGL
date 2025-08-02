//
//  Engine.h
//  LearnOpenGL
//
//  Created by asi on 2024/12/11.
//

#ifndef Engine_h
#define Engine_h

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <map>

#include "Model.h"
#include "Program.h"
#include "Light.hpp"

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
    void createTextures();
    void initScene();
    void initImgui();

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
    
public:
    static Engine engine;
    
private:
    GLFWwindow* m_pWindow;
    
    std::map<std::string, Program*> m_programs;
    std::map<std::string, Model*> m_models;
    std::map<std::string, GLuint> m_VAOs;
    std::map<std::string, GLuint> m_textures;
    
    std::vector<PointLight*> m_pointLights;
    std::vector<FlashLight*> m_flashLights;
    
    PointLight* m_pointLight;
    FlashLight* m_flashLight;
    
    glm::mat4 m_project;
};

#endif /* Engine_h */
