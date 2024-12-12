//
//  Engine.cpp
//  LearnOpenGL
//
//  Created by asi on 2024/12/11.
//

#include <iostream>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "Engine.h"
#include "System.h"
#include "Camera.h"
#include "VerticesData.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define DEFAULT_INDENT 5.f
#define DEFAULT_RADIUS 2.f
#define SEGMENTS 100

void framebufferSizeCallback(GLFWwindow* pWindow, int width, int height);
void mouseCallback(GLFWwindow* pWindow, double x, double y);
void processInput(GLFWwindow* pWindow, Camera* pCamera);
void changeMouseDisplay(GLFWwindow* pWindow, bool bDisplay);
void changeMousePos(GLFWwindow* pWindow, glm::dvec2 mousePos);
void gLClearError();
bool gLCheckError();
void generateCircleVertices(float* vertices, int windowWidth, int windowHeight);

static glm::dvec2 g_mousePos;
static glm::dvec2 g_mouseLastPos;
static std::atomic_bool bDebugging;
const double targetFrameTime = 1.0 / 60.0;

Engine Engine::engine;
Engine::Engine() : m_pWindow(nullptr)
{
    m_fTargetFrameTime = 1.0 / 60.0;
    m_programs.clear();
}

Engine::~Engine()
{
    
}

void Engine::start()
{
    init();
    
    // glfw window creation
    if(!createWindow())
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        return;
    }

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }
    
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_CULL_FACE);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    
    //stbi_set_flip_vertically_on_load(true);
    
    initImgui();
    createSceneThings();
    initScene();
    render();
}

void Engine::init()
{
    // glfw: initialize and configure
    // ------------------------------
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);
}

bool Engine::createWindow()
{
    m_pWindow = nullptr;
    if (false) {
        GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
        if (primaryMonitor) {
            const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
            m_pWindow = glfwCreateWindow(mode->width, mode->height, "FullScreen Example", primaryMonitor, nullptr);
        }
    }
    else
    {
        System::getScreenSize(&System::nScreenWidth, &System::nScreenHeight);
        m_pWindow = glfwCreateWindow(System::nScreenWidth, System::nScreenHeight, "LearnOpenGL", NULL, NULL);
    }
    
    if (m_pWindow != nullptr)
    {
        glfwGetFramebufferSize(m_pWindow, &System::nScreenWidth, &System::nScreenHeight);
        glfwMakeContextCurrent(m_pWindow);
        glfwSetFramebufferSizeCallback(m_pWindow, framebufferSizeCallback);
    }
    else
    {
        glfwTerminate();
        return false;
    }
    return true;
}

void Engine::createSceneThings()
{
    std::vector<std::string> programNames =
    {
        "cube",
        "light",
        "skybox",
        "frontsight",
    };
    
    for (int i = 0; i < programNames.size(); ++i)
    {
        std::string strVertexShader = programNames[i] + ".vs";
        std::string strFragmentShader = programNames[i] + ".fs";
        Program* program = new Program(strVertexShader, strFragmentShader);
        m_programs[programNames[i]] = program;
    }
    
    std::vector<std::pair<std::string, std::string>> modelNamesAndPath =
    {
        {"nanosuit", "nanosuit/nanosuit.obj"},
        {"plane", "plane/wood.obj"},
    };
    
    
    for (int i = 0; i < modelNamesAndPath.size(); ++i) {
        Model model(modelNamesAndPath[i].second);
        m_models[modelNamesAndPath[i].first] = model;
    }


    m_spotLight = new SpotLight(glm::vec3(0.f, 5.0f, 2.0f));
    m_spotLights.push_back(m_spotLight);
    m_flashLight = new FlashLight(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, -1.f));
    //m_flashLight->on = false;
    m_flashLights.push_back(m_flashLight);
    
    createVAOs();
    createTextures();
}

void Engine::createVAOs()
{
    GLuint CubeVAO, CubeVBO;
    glGenVertexArrays(1, &CubeVAO);
    glBindVertexArray(CubeVAO);
    glGenBuffers(1, &CubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, CubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6* sizeof(float)));
    glEnableVertexAttribArray(2);
    m_VAOs["cube"] = CubeVAO;
    
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glBindVertexArray(skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    m_VAOs["skybox"] = skyboxVAO;
    
    float circle[(SEGMENTS + 1) * 2 + 2];
    generateCircleVertices(circle, System::nScreenWidth, System::nScreenHeight);
    unsigned int frontsightVAO, frontsightVBO;
    glGenVertexArrays(1, &frontsightVAO);
    glBindVertexArray(frontsightVAO);
    glGenBuffers(1, &frontsightVBO);
    glBindBuffer(GL_ARRAY_BUFFER, frontsightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circle), circle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    m_VAOs["frontsight"] = frontsightVAO;
    
    glBindVertexArray(0);
}

void Engine::createTextures()
{
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
            std::string filepath = System::resourcePathWithFile("skybox/" + textures_faces[i]);
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
    m_textures["skybox"] = cubeMap;
}

void Engine::initScene()
{
    m_project = glm::perspective(glm::radians(45.f), (float)System::nScreenWidth/(float)System::nScreenHeight, 0.1f, 50.f);
    Camera::main_camera.init(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
    Camera::main_camera.updateAngle(0.f, 0.f);
    
    changeMouseDisplay(m_pWindow, false);
}

void Engine::initImgui()
{
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_pWindow, true);
    ImGui_ImplOpenGL3_Init("#version 410");
    ImGuiIO& io = ImGui::GetIO();
    ImFontConfig font_config;
    font_config.OversampleH = 3;
    font_config.OversampleV = 3;
    font_config.PixelSnapH = true;
    io.Fonts->AddFontFromFileTTF(System::resourcePathWithFile("STIXTwoText-SemiBold.ttf").data(), 12.0f, &font_config, io.Fonts->GetGlyphRangesDefault());
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    io.FontGlobalScale = 1.2f;
    
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4 defaultColor   = ImVec4(.2f, .2f, .2f, 1.f);
    style.GrabMinSize     = 10.f;
    style.GrabRounding    = DEFAULT_RADIUS;
    style.FramePadding    = ImVec2(1.f, 1.f);
    style.FrameRounding   = DEFAULT_RADIUS;
    style.FrameBorderSize = 1.f;
    style.ItemSpacing     = ImVec2(3.f, 5.f);
    style.IndentSpacing   = DEFAULT_INDENT;
    style.WindowPadding   = ImVec2(DEFAULT_INDENT, DEFAULT_INDENT);
    style.Colors[ImGuiCol_Header] = defaultColor;
    style.Colors[ImGuiCol_HeaderHovered] = defaultColor;
    style.Colors[ImGuiCol_HeaderActive] = defaultColor;
    style.Colors[ImGuiCol_FrameBg] = defaultColor;
    style.Colors[ImGuiCol_FrameBgHovered] = defaultColor;
    style.Colors[ImGuiCol_FrameBgActive] = defaultColor;
}

#pragma mark render loop begin
void Engine::render()
{
    if (m_pWindow != nullptr) {
        while (!glfwWindowShouldClose(m_pWindow)) {
            
            double startTime = glfwGetTime();
            
            // check input and update camera
            processInput(m_pWindow, &Camera::main_camera);
            glfwSetCursorPosCallback(m_pWindow, mouseCallback);
            if (!bDebugging) Camera::main_camera.update();

            // clean buffer
            glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            
            m_flashLight->position = Camera::main_camera.getPosition();
            m_flashLight->direction = Camera::main_camera.getForward();
            
            
            
            gLClearError();
            {
                glDepthFunc(GL_LEQUAL);
                Program* skyboxShader = m_programs.at("skybox");
                skyboxShader->use();
                assert(gLCheckError());
                skyboxShader->setUniformMatrix4fv("project", m_project);
                skyboxShader->setUniformMatrix4fv("view", Camera::main_camera.getView());
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_CUBE_MAP, m_textures.at("skybox"));
                glBindVertexArray(m_VAOs.at("skybox"));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                glDepthFunc(GL_LESS);
            }
            
            {
                Program* cubeShader = m_programs.at("cube");
                cubeShader->use();
                cubeShader->setUniform3f("viewPos", Camera::main_camera.getPosition());
                cubeShader->setUniformMatrix4fv("project", m_project);
                cubeShader->setUniformMatrix4fv("view", Camera::main_camera.getView());
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, -.5f, -10.0f));
                cubeShader->setUniformMatrix4fv("model", model);
                cubeShader->setUniformSpotLights(m_spotLights);
                cubeShader->setUniformFlashLight(m_flashLights);
                glStencilFunc(GL_ALWAYS, 1, 0xFF);
                glStencilMask(0xFF);
                Model nanosuit = m_models.at("nanosuit");
                nanosuit.draw(cubeShader);
                Model plane = m_models.at("plane");
                plane.draw(cubeShader);
            }
             

            if (m_spotLight->on)
            {
                // draw light and light cube
                Program* lightShader = m_programs.at("light");
                lightShader->use();
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(0.0f, -.5f, -10.0f));
                lightShader->setUniform3f("lightColor", m_spotLight->color);
                lightShader->setUniformMatrix4fv("model", model);
                lightShader->setUniformMatrix4fv("project", m_project);
                lightShader->setUniformMatrix4fv("view", Camera::main_camera.getView());
                glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
                glStencilMask(0x00);
                glDisable(GL_DEPTH_TEST);

                Model nanosuit = m_models.at("nanosuit");
                nanosuit.draw(lightShader);
                glStencilMask(0xFF);
                glStencilFunc(GL_ALWAYS, 0, 0xFF);
                glEnable(GL_DEPTH_TEST);

                model = glm::mat4(1.0f);
                model = glm::translate(model, m_spotLight->position);
                model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
                lightShader->setUniformMatrix4fv("model", model);
                glBindVertexArray(m_VAOs.at("cube"));
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

            {
                glDisable(GL_DEPTH_TEST);
                Program* frontsightShader = m_programs.at("frontsight");
                frontsightShader->use();
                glBindVertexArray(m_VAOs.at("frontsight"));
                glDrawArrays(GL_TRIANGLE_FAN, 0, SEGMENTS + 2);
                glEnable(GL_DEPTH_TEST);
            }

            assert(gLCheckError());

            renderIngui();
            
            glfwSwapBuffers(m_pWindow);
            glfwPollEvents();
            
            double endTime = glfwGetTime();
            double useTime = endTime - startTime;
            if (useTime < m_fTargetFrameTime) {
                //glfwWaitEventsTimeout(m_fTargetFrameTime - useTime);
            }
        }
        
        
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        
        // glfw: terminate, clearing all previously allocated GLFW resources.
        // ------------------------------------------------------------------
        glfwTerminate();
    }
}
#pragma mark render loop end

void Engine::renderIngui()
{
    // create imgui window
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //ImGui::SetNextWindowCollapsed(!g_bDebug, ImGuiCond_Always);
    ImGui::Begin("Debug");
    if (ImGui::Button("continue", ImVec2(ImGui::GetContentRegionAvail().x / 2.f, 0)))
    {
        glfwSetCursorPos(m_pWindow, System::nScreenWidth / 2, System::nScreenHeight / 2);
        bDebugging = false;
        changeMouseDisplay(m_pWindow, false);
        changeMousePos(m_pWindow, g_mousePos);
        
        ImGuiIO& io = ImGui::GetIO();
        io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
        io.MouseDown[0] = io.MouseDown[1] = io.MouseDown[2] = false;
        ImGui::SetWindowFocus();
    }
    
    if (ImGui::CollapsingHeader("light", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Indent(DEFAULT_INDENT);
        if (ImGui::CollapsingHeader("flash light"))
        {
            ImGui::Checkbox("##enable flash light", &m_flashLight->on);
            ImGui::SameLine();
            ImVec4 selectedColor = ImVec4(m_flashLight->color.x, m_flashLight->color.y, m_flashLight->color.z, 1.0f);
            ImGui::ColorButton("##flash light color", selectedColor);
            ImGui::PushItemWidth(60.0f);
            ImGui::SameLine();
            ImGui::SliderFloat("##flash light r", &m_flashLight->color.x, 0.0f, 1.0f, "%.2f");
            ImGui::SameLine();
            ImGui::SliderFloat("##flash light g", &m_flashLight->color.y, 0.0f, 1.0f, "%.2f");
            ImGui::SameLine();
            ImGui::SliderFloat("##flash light b", &m_flashLight->color.z, 0.0f, 1.0f, "%.2f");
            ImGui::PopItemWidth();
        }
        if (ImGui::CollapsingHeader("spot light 1", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("light pos:");
            ImGui::SameLine();
            float pos[3] = {m_spotLight->position.x, m_spotLight->position.y, m_spotLight->position.z};
            ImGui::DragFloat3("##light pos", pos, 0.02f, -100.f, 100.f, "%.2f");
            m_spotLight->position = glm::vec3(pos[0], pos[1], pos[2]);
            ImGui::Checkbox("##enable spot light", &m_spotLight->on);
            ImGui::SameLine();
            ImVec4 selectedColor = ImVec4(m_spotLight->color.x, m_spotLight->color.y, m_spotLight->color.z, 1.0f);
            ImGui::ColorButton("##spot light color", selectedColor);
            ImGui::PushItemWidth(60.0f);
            ImGui::SameLine();
            ImGui::SliderFloat("##spot light r", &m_spotLight->color.x, 0.0f, 1.0f, "%.2f");
            ImGui::SameLine();
            ImGui::SliderFloat("##spot light g", &m_spotLight->color.y, 0.0f, 1.0f, "%.2f");
            ImGui::SameLine();
            ImGui::SliderFloat("##spot light b", &m_spotLight->color.z, 0.0f, 1.0f, "%.2f");
            ImGui::PopItemWidth();
        }
        ImGui::Unindent(DEFAULT_INDENT);
    }
        
    if (ImGui::CollapsingHeader("output", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Indent(DEFAULT_INDENT);
        ImGui::Text("mouse pos: %.0f %.0f", g_mouseLastPos.x, g_mouseLastPos.y);
        ImGui::Text("view pos: %.2f %.2f %.2f", m_flashLight->position.x, m_flashLight->position.y, m_flashLight->position.z);
        ImGui::Unindent(DEFAULT_INDENT);
    }
    
    
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void framebufferSizeCallback(GLFWwindow* pWindow, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouseCallback(GLFWwindow* pWindow, double x, double y)
{
    // prohibit using mouse in debugging mode
    if (!bDebugging) {
        g_mousePos = glm::vec2(x, y);
        
        static bool firstMouse = true;
        static float lastX     = 0.f;
        static float lastY     = 0.f;
        
        if (firstMouse) {
            lastX      = x;
            lastY      = y;
            firstMouse = false;
        }
        
        float xoffset = x - lastX;
        float yoffset = lastY - y;
        lastX = x;
        lastY = y;

        float sensitivity = 0.02f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;
        Camera::main_camera.updateAngle(xoffset, yoffset);
    }
    else
    {
        g_mouseLastPos = glm::vec2(x, y);
        ImGuiIO& io = ImGui::GetIO();
        io.MousePos = ImVec2((float)x, (float)y);
    }
}

void processInput(GLFWwindow* pWindow, Camera* pCamera)
{
    if (nullptr == pWindow) return;
    
    if (glfwGetKey(pWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(pWindow, true);
    }
    
    if(glfwGetKey(pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS && !bDebugging)
    {
        bDebugging = true;
        changeMouseDisplay(pWindow, bDebugging);
        glfwSetCursorPos(pWindow, System::nScreenWidth / 2, System::nScreenHeight / 2);
    }
    
    if (pCamera == nullptr) return;
    
    if (!bDebugging) {
        if (glfwGetKey(pWindow, GLFW_KEY_W) == GLFW_PRESS) pCamera->move(Camera::FORWARD);
        if (glfwGetKey(pWindow, GLFW_KEY_S) == GLFW_PRESS) pCamera->move(Camera::BACKWARD);
        if (glfwGetKey(pWindow, GLFW_KEY_A) == GLFW_PRESS) pCamera->move(Camera::LEFT);
        if (glfwGetKey(pWindow, GLFW_KEY_D) == GLFW_PRESS) pCamera->move(Camera::RIGHT);
        if (glfwGetKey(pWindow, GLFW_KEY_Q) == GLFW_PRESS) pCamera->move(Camera::UP);
        if (glfwGetKey(pWindow, GLFW_KEY_E) == GLFW_PRESS) pCamera->move(Camera::DOWN);
    }
}

void changeMouseDisplay(GLFWwindow* pWindow, bool bDisplay)
{
    if (nullptr == pWindow) return;
    glfwSetInputMode(pWindow, GLFW_CURSOR, bDisplay ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

void changeMousePos(GLFWwindow* pWindow, glm::dvec2 mousePos)
{
    if (nullptr == pWindow) return;
    glfwSetCursorPos(pWindow, mousePos.x, mousePos.y);
}

void gLClearError()
{
    while (GL_NO_ERROR != glGetError());
}

bool gLCheckError()
{
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] " << error << std::endl;
        return false;
    }
    return true;
}

void generateCircleVertices(float* vertices, int width, int height) {
    const float radius = 0.002f;
    vertices[0] = 0.f;
    vertices[1] = 0.f;

    for (int i = 0; i <= SEGMENTS; i++) {
        float angle = 2.0f * M_PI * i / SEGMENTS;
        float aspectRatio = (float)width / (float)height;
        vertices[2 + i * 2] = cos(angle) * radius ;
        vertices[3 + i * 2] = sin(angle) * radius * aspectRatio;
    }
}
