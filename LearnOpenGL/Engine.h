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
    void render();
    void renderIngui();
    
public:
    static Engine engine;
    
private:
    GLFWwindow* m_pWindow;
    
    double m_fTargetFrameTime;
    
    std::map<std::string, Program*> m_programs;
    std::map<std::string, Model> m_models;
    std::map<std::string, GLuint> m_VAOs;
    std::map<std::string, GLuint> m_textures;
    
    std::vector<SpotLight*> m_spotLights;
    std::vector<FlashLight*> m_flashLights;
    
    SpotLight* m_spotLight;
    FlashLight* m_flashLight;
    
    glm::mat4 m_project;
};

#endif /* Engine_h */
