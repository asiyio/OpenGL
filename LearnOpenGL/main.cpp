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

#include "config.h"
#include "program.h"
#include "System.h"
#include "Camera.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

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
        static bool firstMouse = true;
        static float lastX     = 0.f;
        static float lastY     = 0.f;
        static float pitch     = 0;
        static float yaw       = -90.f;
        
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

        yaw   += xoffset;
        pitch += yoffset;

//        if(pitch > 89.0f)
//            pitch = 89.0f;
//        if(pitch < -89.0f)
//            pitch = -89.0f;
        
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        Camera::main_camera.rotate(front);
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
        glfwGetCursorPos(pWindow, &g_mousePos.x, &g_mousePos.y);
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

static void GLClearError()
{
    while (GL_NO_ERROR != glGetError());
}

static bool GLCheckError()
{
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] " << error << std::endl;
        return false;
    }
    return true;
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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // glfw window creation
    // --------------------
    unsigned int nScreenWidth = 800, nScreenHeight = 600;
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
    change_mouse_display(window, false);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
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
    unsigned char* pData = stbi_load(System::ResourcePathWithFile("container.jpg").data(), &nWidth, &nHeight, &nNrchannel, 0);
    if (pData) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, nWidth, nHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pData);
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

    pData = stbi_load(System::ResourcePathWithFile("awesomeface.png").data(), &nWidth, &nHeight, &nNrchannel, 0);
    if (pData) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nWidth, nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pData);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else{
        std::cout << "texture load fail!" << std::endl;
    }
    stbi_image_free(pData);
    
    glBindVertexArray(0);
    
    Program program("cube_vertex.vs", "cube_fragment.fs");
    Program light("light_vertex.vs", "light_fragment.fs");
    
#pragma mark init imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF(System::ResourcePathWithFile("OpenSans-Regular.ttf").data(), 18.0f);
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    
    ImGui::StyleColorsClassic();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    ImGuiStyle& style = ImGui::GetStyle();
    style.GrabMinSize = 10.f;
    style.GrabRounding = 4.0f;
    glm::mat4 project = glm::perspective(glm::radians(45.f), (float)nScreenWidth/(float)nScreenHeight, 0.1f, 50.f);
    
#pragma mark init camera
    Camera::main_camera.init(glm::vec3(0.f, 0.f, 10.3f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));

    
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

        GLClearError();
        glm::vec3 lightPos(1.2f, 1.0f, -3.0f);
        glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
        glm::vec3 objectColor(0.f, 1.f, 0.f);
        
        {
            // draw wood cube
            program.use();
            program.set_uniform1i("texture1", 0);
            program.set_uniform1i("texture2", 1);
            program.set_uniform3f("lightPos", lightPos);
            program.set_uniform3f("lightColor", lightColor);
            program.set_uniform3f("objectColor", objectColor);
            program.set_uniformMatrix4fv("project", project);
            program.set_uniformMatrix4fv("view", Camera::main_camera.get_view());
            
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture1);
            
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture2);
            
            glBindVertexArray(VAO);
            for(unsigned int i = 0; i < 10; i++)
            {
                glm::mat4 model;
                model = glm::translate(model, cubePositions[i]);
                model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.f, 1.f, 1.f));
                program.set_uniformMatrix4fv("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }
        
        {
            // draw light and light cube
            light.use();
            glm::mat4 model;
            model = glm::translate(model, lightPos);
            model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
            
            light.set_uniform3f("lightColor", lightColor);
            light.set_uniformMatrix4fv("model", model);
            light.set_uniformMatrix4fv("project", project);
            light.set_uniformMatrix4fv("view", Camera::main_camera.get_view());
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
            change_mouse_pos(window, g_mousePos);
            change_mouse_display(window, false);
            g_bDebug = false;
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
