//
//  main.cpp
//  LearnOpenGL
//
//  Created by Rocky on 2024/7/27.
//
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <string>
#include <cassert>
#include <vector>
#include <cmath>

#include "config.h"
#include "program.h"
#include "System.h"
#include "Camera.h"
#include "Material.hpp"
#include "Light.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define DEFAULTINDENT 5.f
#define DEFAULTRADIUS 2.f

static bool g_bDebug = false;
static glm::dvec2 g_mousePos;

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
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2((float)xpos, (float)ypos);
    
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
}

void processInput(GLFWwindow* pWindow, Camera* pCamera)
{
    if (nullptr == pWindow) return;
    
    if (glfwGetKey(pWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(pWindow, true);
    }
    
    if(glfwGetKey(pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        g_bDebug = true;
        change_mouse_display(pWindow, g_bDebug);
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
    //io.Fonts->AddFontFromFileTTF(System::ResourcePathWithFile("OpenSans-Regular.ttf").data(), 18.0f);
    io.Fonts->AddFontFromFileTTF(System::ResourcePathWithFile("OpenSans-SemiBold.ttf").data(), 16.0f);
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");
    
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4 defaultColor   = ImVec4(.2f, .2f, .2f, 1.f);
    style.GrabMinSize     = 12.f;
    style.GrabRounding    = DEFAULTRADIUS;
    style.FramePadding    = ImVec2(1.f, 1.f);
    style.FrameRounding   = DEFAULTRADIUS;
    style.FrameBorderSize = 1.f;
    style.ItemSpacing     = ImVec2(3.f, 3.f);
    style.IndentSpacing   = DEFAULTINDENT;
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

glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f),
    glm::vec3( 2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3( 2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3( 1.3f, -2.0f, -2.5f),
    glm::vec3( 1.5f,  2.0f, -2.5f),
    glm::vec3( 1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
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

    // glfw window creation
    // --------------------
    unsigned int nScreenWidth = 1200, nScreenHeight = 800;
    System::GetScreenSize(&nScreenWidth, &nScreenHeight);
    GLFWwindow* window = glfwCreateWindow(nScreenWidth, nScreenHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
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
    
    stbi_set_flip_vertically_on_load(true);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
//    unsigned int EBO;
//    glGenBuffers(1, &EBO);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6* sizeof(float)));
    glEnableVertexAttribArray(2);
    
    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    int nWidth, nHeight, nNrchannel;
    unsigned char* pData = stbi_load(System::ResourcePathWithFile("container2.png").data(), &nWidth, &nHeight, &nNrchannel, 0);
    if (pData) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nWidth, nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pData);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else{
        std::cout << "texture load fail!" << std::endl;
    }
    stbi_image_free(pData);
    pData = nullptr;
    
    unsigned int texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    pData = stbi_load(System::ResourcePathWithFile("container2_specular.png").data(), &nWidth, &nHeight, &nNrchannel, 0);
    if (pData) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nWidth, nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pData);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else{
        std::cout << "texture load fail!" << std::endl;
    }
    stbi_image_free(pData);
    
    glBindVertexArray(0);
    
    Material material(texture1, texture2, 32.0f);
    Program cubeShader("cube_vertex.vert", "cube_fragment.frag");
    Program lightShader("light_vertex.vs", "light_fragment.fs");
    
#pragma mark init screen
    glm::mat4 project = glm::perspective(glm::radians(45.f), (float)nScreenWidth/(float)nScreenHeight, 0.1f, 50.f);
    Camera::main_camera.init(glm::vec3(0.f, 0.f, 10.3f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
    
    std::vector<SpotLight*> spotLights;
    SpotLight* spLight = new SpotLight(glm::vec3(1.2f, 1.0f, -3.0f));
    spotLights.push_back(spLight);
    
    std::vector<FlashLight*> flashLights;
    FlashLight* flLight = new FlashLight(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, -1.f));
    flLight->on = false;
    flashLights.push_back(flLight);
    
    glfwPollEvents();
    change_mouse_display(window, false);
#pragma mark render loop
    while (!glfwWindowShouldClose(window))
    {
        // update camera
        if (!g_bDebug) Camera::main_camera.update();
        
        // check input
        processInput(window, &Camera::main_camera);
        glfwSetCursorPosCallback(window, mouse_callback);
        
        // clean buffer
        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        flLight->position = Camera::main_camera.get_position();
        flLight->direction = Camera::main_camera.get_forward();

        GLClearError();

        {
            // bind texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, material.diffuse);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, material.specular);
            
            // draw wood cube
            cubeShader.use();
            cubeShader.set_uniform3f("viewPos", Camera::main_camera.get_position());
            cubeShader.set_uniformMatrix4fv("project", project);
            cubeShader.set_uniformMatrix4fv("view", Camera::main_camera.get_view());
            
            cubeShader.set_uniform1i("material.diffuse", 0);
            cubeShader.set_uniform1i("material.specular", 1);
            cubeShader.set_uniform1f("material.shininess", material.shininess);
            
            cubeShader.set_uniformSpotLights(spotLights);
            cubeShader.set_uniformFlashLight(flashLights);

            glBindVertexArray(VAO);
            for(unsigned int i = 0; i < 10; i++)
            {
                glm::mat4 model;
                model = glm::translate(model, cubePositions[i]);
                model = glm::rotate(model, float(std::pow(-1, i % 2 )) * (float)glfwGetTime(), glm::vec3(1.f, 1.f, 1.f));
                cubeShader.set_uniformMatrix4fv("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }
        
        if (spLight->on)
        {
            // draw light and light cube
            lightShader.use();
            glm::mat4 model;
            model = glm::translate(model, spLight->position);
            model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
            
            lightShader.set_uniform3f("lightColor", spLight->color);
            lightShader.set_uniformMatrix4fv("model", model);
            lightShader.set_uniformMatrix4fv("project", project);
            lightShader.set_uniformMatrix4fv("view", Camera::main_camera.get_view());
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        assert(GLCheckError());
        
        // create imgui window
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Debug Window");
        if (ImGui::Button("continue"))
        {
            change_mouse_display(window, false);
            change_mouse_pos(window, g_mousePos);
            g_bDebug = false;
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
            ImGui::Text("pos x:%f y:%f z:%f", flLight->position.x, flLight->position.y, flLight->position.z);
            ImGui::Unindent(DEFAULTINDENT);
        }
        
        
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
