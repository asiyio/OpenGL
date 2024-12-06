//
//  main.cpp
//  LearnOpenGL
//
//  Created by Rocky on 2024/7/27.
//
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <string>
#include <cassert>
#include <vector>
#include <cmath>

#include "config.h"
#include "Program.h"
#include "System.h"
#include "Camera.h"
#include "Material.hpp"
#include "Light.hpp"
#include "Mesh.h"
#include "Model.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define DEFAULTINDENT 5.f
#define DEFAULTRADIUS 2.f


static std::atomic_bool g_bDebug = false;
static glm::dvec2 g_mousePos;
static glm::dvec2 g_mouseLastPos;

void change_mouse_display(GLFWwindow* pWindow, bool bDisplay)
{
    if (nullptr == pWindow) return;
    glfwSetInputMode(pWindow, GLFW_CURSOR, bDisplay ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

void change_mouse_pos(GLFWwindow* pWindow, glm::dvec2 mousePos)
{
    if (nullptr == pWindow) return;
    glfwSetCursorPos(pWindow, mousePos.x, mousePos.y);
}

void framebuffer_size_callback(GLFWwindow* pWindow, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* pWindow, double xpos, double ypos)
{
    // prohibit using mouse in debugging mode
    if (!g_bDebug) {
        g_mousePos = glm::vec2(xpos, ypos);
        
        static bool firstMouse = true;
        static float lastX     = 0.f;
        static float lastY     = 0.f;
        
        if (firstMouse) {
            lastX      = xpos;
            lastY      = ypos;
            firstMouse = false;
        }
        
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;

        float sensitivity = 0.02f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;
        Camera::main_camera.update_angle(xoffset, yoffset);
    }
    else
    {
        g_mouseLastPos = glm::vec2(xpos, ypos);
        ImGuiIO& io = ImGui::GetIO();
        io.MousePos = ImVec2((float)xpos, (float)ypos);
    }
}

void processInput(GLFWwindow* pWindow, Camera* pCamera)
{
    if (nullptr == pWindow) return;
    
    if (glfwGetKey(pWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(pWindow, true);
    }
    
    if(glfwGetKey(pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS && !g_bDebug)
    {
        g_bDebug = true;
        change_mouse_display(pWindow, g_bDebug);
        glfwSetCursorPos(pWindow, System::nScreenWidth / 2, System::nScreenHeight / 2);
    }
    
    if (pCamera == nullptr) return;
    
    if (!g_bDebug) {
        if (glfwGetKey(pWindow, GLFW_KEY_W) == GLFW_PRESS) pCamera->move(Camera::FORWARD);
        if (glfwGetKey(pWindow, GLFW_KEY_S) == GLFW_PRESS) pCamera->move(Camera::BACKWARD);
        if (glfwGetKey(pWindow, GLFW_KEY_A) == GLFW_PRESS) pCamera->move(Camera::LEFT);
        if (glfwGetKey(pWindow, GLFW_KEY_D) == GLFW_PRESS) pCamera->move(Camera::RIGHT);
        if (glfwGetKey(pWindow, GLFW_KEY_Q) == GLFW_PRESS) pCamera->move(Camera::UP);
        if (glfwGetKey(pWindow, GLFW_KEY_E) == GLFW_PRESS) pCamera->move(Camera::DOWN);
    }
}

void GLClearError()
{
    while (GL_NO_ERROR != glGetError());
}

bool GLCheckError()
{
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] " << error << std::endl;
        return false;
    }
    return true;
}

void init_imgui(GLFWwindow* window)
{
    if (nullptr == window) return;
#pragma mark init imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImFontConfig font_config;
    font_config.OversampleH = 3;
    font_config.OversampleV = 3;
    font_config.PixelSnapH = true;
    io.Fonts->AddFontFromFileTTF(System::ResourcePathWithFile("STIXTwoText-SemiBold.ttf").data(), 12.0f, &font_config, io.Fonts->GetGlyphRangesDefault());
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    io.FontGlobalScale = 1.2f;
    
    
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");
    
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4 defaultColor   = ImVec4(.2f, .2f, .2f, 1.f);
    style.GrabMinSize     = 10.f;
    style.GrabRounding    = DEFAULTRADIUS;
    style.FramePadding    = ImVec2(1.f, 1.f);
    style.FrameRounding   = DEFAULTRADIUS;
    style.FrameBorderSize = 1.f;
    style.ItemSpacing     = ImVec2(3.f, 5.f);
    style.IndentSpacing   = DEFAULTINDENT;
    style.WindowPadding   = ImVec2(DEFAULTINDENT, DEFAULTINDENT);
    style.Colors[ImGuiCol_Header] = defaultColor;
    style.Colors[ImGuiCol_HeaderHovered] = defaultColor;
    style.Colors[ImGuiCol_HeaderActive] = defaultColor;
    style.Colors[ImGuiCol_FrameBg] = defaultColor;
    style.Colors[ImGuiCol_FrameBgHovered] = defaultColor;
    style.Colors[ImGuiCol_FrameBgActive] = defaultColor;
}

// cube vertex
float vertices[] = {
    // pos                normal              textCoord
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
 
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
 
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
 
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
 
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
 
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f
};

float skyboxVertices[] = {
    // positions
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    //GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    //const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
    //GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "FullScreen Example", primaryMonitor, nullptr);
    // glfw window creation
    // --------------------
    System::GetScreenSize(&System::nScreenWidth, &System::nScreenHeight);
    GLFWwindow* window = glfwCreateWindow(System::nScreenWidth, System::nScreenHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwGetFramebufferSize(window, &System::nScreenWidth, &System::nScreenHeight);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // initial imgui context and set imgui style
    init_imgui(window);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    
    //stbi_set_flip_vertically_on_load(true);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6* sizeof(float)));
    glEnableVertexAttribArray(2);
    
    unsigned int skyboxVAO;
    glGenVertexArrays(1, &skyboxVAO);
    glBindVertexArray(skyboxVAO);
    
    unsigned int skyboxVBO;
    glGenBuffers(1, &skyboxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
    
    Program cubeShader("cube_vertex.vert", "cube_fragment.frag");
    Program lightShader("light_vertex.vs", "light_fragment.fs");
    Program skyboxShader("skybox_vertex.vert", "skybox_fragment.frag");
    
    Model suit("nanosuit/nanosuit.obj");
    
    unsigned int cubeMap;
    std::vector<std::string> textures_faces = {"right.jpg", "left.jpg", "top.jpg", "bottom.jpg", "front.jpg", "back.jpg"};
    {
        // load cubemaps
        
        glGenTextures(1, &cubeMap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
        int width, height, nrChannels;
        unsigned char *data;
        for(unsigned int i = 0; i < textures_faces.size(); i++)
        {
            std::string filepath = System::ResourcePathWithFile("skybox/" + textures_faces[i]);
            data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
            if (data)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
    
#pragma mark init screen
    glm::mat4 project = glm::perspective(glm::radians(45.f), (float)System::nScreenWidth/(float)System::nScreenHeight, 0.1f, 50.f);
    Camera::main_camera.init(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
    
    std::vector<SpotLight*> spotLights;
    SpotLight* spLight = new SpotLight(glm::vec3(0.f, 5.0f, 2.0f));
    spotLights.push_back(spLight);
    
    std::vector<FlashLight*> flashLights;
    FlashLight* flLight = new FlashLight(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, -1.f));
    flLight->on = false;
    flashLights.push_back(flLight);
    
    glfwPollEvents();
    change_mouse_display(window, false);
    
    const double targetFrameTime = 1.0 / 60.0;
#pragma mark render loop
    while (!glfwWindowShouldClose(window))
    {
        double frameStartTime = glfwGetTime();
        // update camera
        if (!g_bDebug)
        {
            Camera::main_camera.update();
        }
        
        // check input
        processInput(window, &Camera::main_camera);
        glfwSetCursorPosCallback(window, mouse_callback);
        
        // clean buffer
        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        
        flLight->position = Camera::main_camera.get_position();
        flLight->direction = Camera::main_camera.get_forward();

        GLClearError();
        
#pragma mark [first step] draw skybox
        {
            glDepthFunc(GL_LEQUAL);
            skyboxShader.use();
            skyboxShader.set_uniformMatrix4fv("project", project);
            glm::mat4 pos = Camera::main_camera.get_view();
            skyboxShader.set_uniformMatrix4fv("view", Camera::main_camera.get_view());
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
            glBindVertexArray(skyboxVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glDepthFunc(GL_LESS);
        }
        
        {
            cubeShader.use();
            cubeShader.set_uniform3f("viewPos", Camera::main_camera.get_position());
            cubeShader.set_uniformMatrix4fv("project", project);
            cubeShader.set_uniformMatrix4fv("view", Camera::main_camera.get_view());
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, -.5f, -10.0f));
            cubeShader.set_uniformMatrix4fv("model", model);
            cubeShader.set_uniformSpotLights(spotLights);
            cubeShader.set_uniformFlashLight(flashLights);
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilMask(0xFF);
            suit.draw(cubeShader);
        }
        
        if (spLight->on)
        {
            // draw light and light cube
            lightShader.use();
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, -.5f, -10.0f));
            //model = glm::scale(model, glm::vec3(1.1f, 1.1f, 1.1f));
            lightShader.set_uniform3f("lightColor", spLight->color);
            lightShader.set_uniformMatrix4fv("model", model);
            lightShader.set_uniformMatrix4fv("project", project);
            lightShader.set_uniformMatrix4fv("view", Camera::main_camera.get_view());
            glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
            glStencilMask(0x00);
            glDisable(GL_DEPTH_TEST);
            suit.draw(lightShader);
            assert(GLCheckError());
            glStencilMask(0xFF);
            glStencilFunc(GL_ALWAYS, 0, 0xFF);
            glEnable(GL_DEPTH_TEST);
            
            model = glm::mat4(1.0f);
            model = glm::translate(model, spLight->position);
            model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
            //lightShader.set_uniform3f("lightColor", spLight->color);
            lightShader.set_uniformMatrix4fv("model", model);
            //lightShader.set_uniformMatrix4fv("project", project);
            //lightShader.set_uniformMatrix4fv("view", Camera::main_camera.get_view());
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        assert(GLCheckError());
        
        // create imgui window
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //ImGui::SetNextWindowCollapsed(!g_bDebug, ImGuiCond_Always);
        ImGui::Begin("Debug");
        if (ImGui::Button("continue", ImVec2(ImGui::GetContentRegionAvail().x / 2.f, 0)))
        {
            glfwSetCursorPos(window, System::nScreenWidth / 2, System::nScreenHeight / 2);
            g_bDebug = false;
            change_mouse_display(window, false);
            change_mouse_pos(window, g_mousePos);
            
            ImGuiIO& io = ImGui::GetIO();
            io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
            io.MouseDown[0] = io.MouseDown[1] = io.MouseDown[2] = false;
            ImGui::SetWindowFocus();
        }
        
        if (ImGui::CollapsingHeader("light", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Indent(DEFAULTINDENT);
            if (ImGui::CollapsingHeader("flash light"))
            {
                ImGui::Checkbox("##enable flash light", &flLight->on);
                ImGui::SameLine();
                ImVec4 selectedColor = ImVec4(flLight->color.x, flLight->color.y, flLight->color.z, 1.0f);
                ImGui::ColorButton("##flash light color", selectedColor);
                ImGui::PushItemWidth(60.0f);
                ImGui::SameLine();
                ImGui::SliderFloat("##flash light r", &flLight->color.x, 0.0f, 1.0f, "%.2f");
                ImGui::SameLine();
                ImGui::SliderFloat("##flash light g", &flLight->color.y, 0.0f, 1.0f, "%.2f");
                ImGui::SameLine();
                ImGui::SliderFloat("##flash light b", &flLight->color.z, 0.0f, 1.0f, "%.2f");
                ImGui::PopItemWidth();
            }
            if (ImGui::CollapsingHeader("spot light 1", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Text("light pos:");
                ImGui::SameLine();
                float pos[3] = {spLight->position.x, spLight->position.y, spLight->position.z};
                ImGui::DragFloat3("##light pos", pos, 0.02f, -100.f, 100.f, "%.2f");
                spLight->position = glm::vec3(pos[0], pos[1], pos[2]);
                ImGui::Checkbox("##enable spot light", &spLight->on);
                ImGui::SameLine();
                ImVec4 selectedColor = ImVec4(spLight->color.x, spLight->color.y, spLight->color.z, 1.0f);
                ImGui::ColorButton("##spot light color", selectedColor);
                ImGui::PushItemWidth(60.0f);
                ImGui::SameLine();
                ImGui::SliderFloat("##spot light r", &spLight->color.x, 0.0f, 1.0f, "%.2f");
                ImGui::SameLine();
                ImGui::SliderFloat("##spot light g", &spLight->color.y, 0.0f, 1.0f, "%.2f");
                ImGui::SameLine();
                ImGui::SliderFloat("##spot light b", &spLight->color.z, 0.0f, 1.0f, "%.2f");
                ImGui::PopItemWidth();
            }
            ImGui::Unindent(DEFAULTINDENT);
        }
            
        if (ImGui::CollapsingHeader("output", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Indent(DEFAULTINDENT);
            ImGui::Text("mouse pos: %.0f %.0f", g_mouseLastPos.x, g_mouseLastPos.y);
            ImGui::Text("view pos: %.2f %.2f %.2f", flLight->position.x, flLight->position.y, flLight->position.z);
            ImGui::Unindent(DEFAULTINDENT);
        }
        
        
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        double frameEndTime = glfwGetTime();
        double frameTime = frameEndTime - frameStartTime;
        if (frameTime < targetFrameTime) {
            glfwWaitEventsTimeout(targetFrameTime - frameTime);
        }
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
