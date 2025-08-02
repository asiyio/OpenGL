//
//  Engine.cpp
//  LearnOpenGL
//
//  Created by asi on 2024/12/11.
//

#ifdef _WIN32
#include <corecrt_math_defines.h>
#endif

#include "Camera.h"
#include "Config.h"
#include "Engine.h"
#include "Material.hpp"
#include "System.hpp"
#include "VerticesData.h"
#include <chrono>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <iostream>
#include <thread>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define DEFAULT_INDENT 5.f
#define DEFAULT_RADIUS 2.f
#define SEGMENTS 100

// 深度贴图尺寸
#define DEPTHBUFFERWIDTH 1024
#define DEPTHBUFFERHEIGHT 1024

// 阴影深度贴图
GLuint pingDepthMap, pongDepthMap;
GLuint pingFBO, pongFBO;

void framebufferSizeCallback(GLFWwindow *pWindow, int width, int height);
void mouseCallback(GLFWwindow *pWindow, double x, double y);
void processInput(GLFWwindow *pWindow, Camera *pCamera);
void changeMouseDisplay(GLFWwindow *pWindow, bool bDisplay);
void changeMousePos(GLFWwindow *pWindow, glm::dvec2 mousePos);
void gLClearError();
bool gLCheckError();
void generateCircleVertices(float *vertices, int windowWidth, int windowHeight);

static glm::dvec2 g_mousePos;
static glm::dvec2 g_mouseLastPos;
static std::atomic_bool bDebugging;
const double targetFrameTime = 1.0 / 60.0;

float quadVertices[] = {
    // positions   // texCoords
    -1.0f,
    -1.0f,
    0.0f,
    0.0f,
    1.0f,
    -1.0f,
    1.0f,
    0.0f,
    -1.0f,
    1.0f,
    0.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
};

Engine Engine::engine;
Engine::Engine()
{
    m_pWindow = nullptr;
    m_pointLight = nullptr;
    m_flashLight = nullptr;
    m_programs.clear();
}

Engine::~Engine()
{
}

void Engine::start()
{
    init();

    // glfw window creation
    if (!createWindow())
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
    // glEnable(GL_CULL_FACE);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glfwSwapInterval(0);

    // stbi_set_flip_vertically_on_load(true);
    Material::loadMaterial();

    initImgui();
    createSceneThings();
    initScene();
    renderLoop();
}

void Engine::init()
{
    // glfw: initialize and configure
    // ------------------------------
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwSwapInterval(0);
}

bool Engine::createWindow()
{
    m_pWindow = nullptr;
    if (false)
    {
        GLFWmonitor *primaryMonitor = glfwGetPrimaryMonitor();
        if (primaryMonitor)
        {
            const GLFWvidmode *mode = glfwGetVideoMode(primaryMonitor);
            m_pWindow = glfwCreateWindow(mode->width, mode->height, "FullScreen Example", primaryMonitor, nullptr);
        }
    }
    else
    {
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
            //"cube",
            //"light",
            //"skybox",
            //"frontsight",
            //"shadow",
            //"blurshadow",
            //"shadowoutput",
            //"bulb",
            //"outlining_effect",

            "light", "skybox", "frontsight", "cook-torrance"};

    for (int i = 0; i < programNames.size(); ++i)
    {
        Program *program = new Program(programNames[i]);
        m_programs[programNames[i]] = program;
    }

    // 初始化加载模型
    std::vector<std::pair<std::string, std::string>> modelNamesAndPath =
        {
            //{"nanosuit", "nanosuit/nanosuit.obj"},
            //{"plane", "plane/plane.obj"},
            //{"bowlingball", "bowlingball/bowlingball.obj"},
            {"pool-ball", "pool-ball/pool-ball.obj"},
            //{"bulb", "bulb/bulb_body.obj"},
        };

    for (int i = 0; i < modelNamesAndPath.size(); ++i)
    {
        Model *model = new Model(modelNamesAndPath[i].second);
        model->init();
        m_models[modelNamesAndPath[i].first] = model;
    }

    // 放到线程中去初始化模型
    //     std::thread t([&]() {
    //         for (auto& model : m_models)
    //         {
    //             model.second->init();
    //         }
    //     });
    //     t.join();

    m_pointLight = new PointLight(glm::vec3(2.42f, 1.7f, -1.5f));
    m_pointLights.push_back(m_pointLight);
    m_flashLight = new FlashLight(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, -1.f));
    m_flashLight->on = false;
    m_flashLights.push_back(m_flashLight);

    createVAOs();
    createDepthBuffer();
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    m_VAOs["cube"] = CubeVAO;

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glBindVertexArray(skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
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
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
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
        for (unsigned int i = 0; i < textures_faces.size(); i++)
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
    m_project = glm::perspective(glm::radians(45.f), (float)System::nScreenWidth / (float)System::nScreenHeight, 0.1f, 500.f);
    Camera::main_camera.init(glm::vec3(11.f, 3.2f, 10.5f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
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
    ImGuiIO &io = ImGui::GetIO();
    ImFontConfig font_config;
    font_config.OversampleH = 3;
    font_config.OversampleV = 3;
    font_config.PixelSnapH = true;
    io.Fonts->AddFontFromFileTTF(System::resourcePathWithFile("font/NotoSans-Bold.ttf").data(), 14.0f, &font_config, io.Fonts->GetGlyphRangesDefault());
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    io.FontGlobalScale = 1.2f;

    ImGuiStyle &style = ImGui::GetStyle();
    ImVec4 defaultColor = ImVec4(.2f, .2f, .2f, 1.f);
    style.GrabMinSize = 10.f;
    style.GrabRounding = DEFAULT_RADIUS;
    style.FramePadding = ImVec2(1.f, 1.f);
    style.FrameRounding = DEFAULT_RADIUS;
    style.FrameBorderSize = 1.f;
    style.ItemSpacing = ImVec2(3.f, 5.f);
    style.IndentSpacing = DEFAULT_INDENT;
    style.WindowPadding = ImVec2(DEFAULT_INDENT, DEFAULT_INDENT);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.f, 0.f, 0.f, 1.f);
    style.Colors[ImGuiCol_Header] = defaultColor;
    style.Colors[ImGuiCol_HeaderHovered] = defaultColor;
    style.Colors[ImGuiCol_HeaderActive] = defaultColor;
    style.Colors[ImGuiCol_FrameBg] = defaultColor;
    style.Colors[ImGuiCol_FrameBgHovered] = defaultColor;
    style.Colors[ImGuiCol_FrameBgActive] = defaultColor;
}

#pragma region Render
void Engine::renderLoop()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    double lastTime = 0.0;
    int fCount = 0;

    // 设置模型位置
    // Model* nanosuit = m_models.at("nanosuit");
    // nanosuit->setPosition(glm::vec3(0.f, 5.f, -5.0f));

    // Model* plane = m_models.at("plane");

    if (m_pWindow != nullptr)
    {
        while (!glfwWindowShouldClose(m_pWindow))
        {

            double currentTime = glfwGetTime();
            double deltaTime = currentTime - lastTime;
            fCount++;
            if (deltaTime >= 1.0)
            {
                frameCount = int(fCount / deltaTime);
                fCount = 0;
                lastTime = currentTime;
            }

            // === 阶段 1: 处理鼠标、键盘事件 ===
            processInput(m_pWindow, &Camera::main_camera);
            glfwSetCursorPosCallback(m_pWindow, mouseCallback);
            if (!bDebugging)
                Camera::main_camera.update();
            m_flashLight->position = Camera::main_camera.pos();
            m_flashLight->direction = Camera::main_camera.forward();
            // === 阶段 2: 渲染深度贴图 ===
            // renderDepthBuffer();
            // === 阶段 3: 渲染场景 ===
            renderScreen();
            // === 阶段 4: 渲染菜单 ===
            renderIngui();

            glfwSwapBuffers(m_pWindow);
            glfwPollEvents();
        }

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        // glfw: terminate, clearing all previously allocated GLFW resources.
        // ------------------------------------------------------------------
        glfwTerminate();
    }
}

void Engine::createDepthBuffer()
{
    // 创建深度贴图
    glGenTextures(1, &pingDepthMap);
    glGenTextures(1, &pongDepthMap);

    // 为立方体贴图的 6 个面分配空间，并设置纹理参数
    glBindTexture(GL_TEXTURE_CUBE_MAP, pingDepthMap);
    for (GLuint i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                     DEPTHBUFFERWIDTH, DEPTHBUFFERHEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, pongDepthMap);
    for (GLuint i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                     DEPTHBUFFERWIDTH, DEPTHBUFFERHEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_NONE);

    glGenFramebuffers(1, &pingFBO);
    glGenFramebuffers(1, &pongFBO);
}

void Engine::renderDepthBuffer()
{
    // 为点光源的 6 个方向设置视角矩阵和投影矩阵
    float nearPlane = 1.0f, farPlane = 100.f;
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, nearPlane, farPlane);
    glm::vec3 lightPos = m_pointLight->position;
    std::vector<glm::mat4> shadowTransforms;
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
    if (m_pointLight->on)
    {
        // 第一步：渲染到 pingDepthMap
        gLClearError();
        glViewport(0, 0, DEPTHBUFFERWIDTH, DEPTHBUFFERHEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, pingFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        Program *shadowShader = m_programs.at("shadow");
        shadowShader->use();
        shadowShader->setUniform3f("lightPos", lightPos);
        shadowShader->setUniform1f("far_plane", farPlane);
        shadowShader->setUniformMatrix4fv("shadowMatrices", shadowTransforms);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, pingDepthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        //         if (nanosuit->loadFinished())
        //         {
        //             glm::mat4 model = glm::translate(glm::mat4(1.0f), nanosuit->getPosition());
        //             shadowShader->setUniformMatrix4fv("model", model);
        //             //nanosuit->draw(shadowShader);
        //         }
        //
        //         if (plane->loadFinished())
        //         {
        //             glm::mat4 model = glm::translate(glm::mat4(1.0f), plane->getPosition());
        //             shadowShader->setUniformMatrix4fv("model", model);
        //             //plane->draw(shadowShader);
        //         }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        assert(gLCheckError());

        // 第二步：模糊到 pongDepthMap
        gLClearError();
        glViewport(0, 0, DEPTHBUFFERWIDTH, DEPTHBUFFERHEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, pongFBO);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glClear(GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE11);
        glBindTexture(GL_TEXTURE_CUBE_MAP, pingDepthMap);
        Program *blurshadowShader = m_programs.at("blurshadow");
        blurshadowShader->use();
        blurshadowShader->setUniform1i("sourceDepthMap", 11);

        GLuint quadVAO, quadVBO;
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));

        for (int i = 0; i < 6; ++i)
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, pongDepthMap, 0);
            glClear(GL_DEPTH_BUFFER_BIT);
            blurshadowShader->setUniform1i("faceIndex", i);
            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
        assert(gLCheckError());
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteVertexArrays(1, &quadVAO);
        glDeleteBuffers(1, &quadVBO);
    }
}

void Engine::renderScreen()
{
    glViewport(0, 0, System::nScreenWidth, System::nScreenHeight);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    if (renderSkyBox)
    {
        glDepthFunc(GL_LEQUAL);
        Program *skyboxShader = m_programs.at("skybox");
        skyboxShader->use();
        skyboxShader->setUniformMatrix4fv("project", m_project);
        skyboxShader->setUniformMatrix4fv("view", Camera::main_camera.view());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_textures.at("skybox"));
        glBindVertexArray(m_VAOs.at("skybox"));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS);
    }

    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glEnable(GL_DEPTH_TEST);

    // 渲染模型
    renderCookTorrancePBR();

    if (m_pointLight->on)
    {
        // draw light and light cube
        Program *lightShader = m_programs.at("light");
        lightShader->use();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -.5f, -10.0f));
        lightShader->setUniform3f("lightColor", m_pointLight->color * m_pointLight->intensity);
        lightShader->setUniformMatrix4fv("model", model);
        lightShader->setUniformMatrix4fv("project", m_project);
        lightShader->setUniformMatrix4fv("view", Camera::main_camera.view());

        model = glm::mat4(1.0f);
        model = glm::translate(model, m_pointLight->position);
        model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
        lightShader->setUniformMatrix4fv("model", model);
        glBindVertexArray(m_VAOs.at("cube"));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }

    //     {
    //         glDisable(GL_DEPTH_TEST);
    //         Program* frontsightShader = m_programs.at("frontsight");
    //         frontsightShader->use();
    //         glBindVertexArray(m_VAOs.at("frontsight"));
    //         glDrawArrays(GL_TRIANGLE_FAN, 0, SEGMENTS + 2);
    //         glBindVertexArray(0);
    //         glEnable(GL_DEPTH_TEST);
    //     }
}

void Engine::renderIngui()
{
    // create imgui window
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // ImGui::SetNextWindowCollapsed(!g_bDebug, ImGuiCond_Always);
    ImGui::Begin("Debug");
    if (ImGui::Button("continue", ImVec2(ImGui::GetContentRegionAvail().x / 2.f, 0)))
    {
        glfwSetCursorPos(m_pWindow, System::nScreenWidth / 2, System::nScreenHeight / 2);
        bDebugging = false;
        changeMouseDisplay(m_pWindow, false);
        changeMousePos(m_pWindow, g_mousePos);

        ImGuiIO &io = ImGui::GetIO();
        io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
        io.MouseDown[0] = io.MouseDown[1] = io.MouseDown[2] = false;
        ImGui::SetWindowFocus();
    }

    if (ImGui::CollapsingHeader("cook-torrance PBR material", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("enable cook-torrance PBR:");
        ImGui::SameLine();
        ImGui::Checkbox("##enable cook-torrance PBR", &renderCTPBR);
        std::vector<std::string> materialNames;
        for (const auto &material : Material::materials)
        {
            materialNames.push_back(material.name);
        }
        ImGui::PushItemWidth(150.0f);
        ImGui::Text("material:");
        ImGui::SameLine();
        if (Material::cIndex < Material::materials.size())
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 0.0f));
            if (ImGui::BeginCombo("##material", Material::cCT_PBR.name.data()))
            {
                for (int n = 0; n < materialNames.size(); n++)
                {
                    bool is_selected = (Material::cIndex == n);
                    if (ImGui::Selectable(materialNames[n].c_str(), is_selected))
                    {
                        Material::cIndex = n;
                        Material::cCT_PBR = Material::materials[Material::cIndex];
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::PopStyleVar();
        }
        ImGui::PopItemWidth();

        ImGui::Text("albedo color:");
        ImGui::SameLine();
        ImGui::ColorButton("##albedo color btn", ImVec4(Material::cCT_PBR.albedo.x, Material::cCT_PBR.albedo.y, Material::cCT_PBR.albedo.z, 1.0f));
        ImGui::PushItemWidth(80.0f);
        ImGui::SameLine();
        ImGui::SliderFloat("##albedo color r", &Material::cCT_PBR.albedo.x, 0.0f, 1.0f, "%.2f");
        ImGui::SameLine();
        ImGui::SliderFloat("##albedo color g", &Material::cCT_PBR.albedo.y, 0.0f, 1.0f, "%.2f");
        ImGui::SameLine();
        ImGui::SliderFloat("##albedo color b", &Material::cCT_PBR.albedo.z, 0.0f, 1.0f, "%.2f");

        ImGui::Text("metallic:");
        ImGui::SameLine();
        ImGui::SliderFloat("##metallic", &Material::cCT_PBR.metallic, 0.f, 1.f, "%.2f");
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(5.0f, 0));
        ImGui::SameLine();
        ImGui::Text("roughness:");
        ImGui::SameLine();
        ImGui::SliderFloat("##roughness", &Material::cCT_PBR.roughness, 0.0f, 1.f, "%.2f");
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(5.0f, 0));
        ImGui::SameLine();
        ImGui::Text("ao:");
        ImGui::SameLine();
        ImGui::SliderFloat("##ao", &Material::cCT_PBR.ao, 0.1f, 1.f, "%.2f");
        ImGui::PopItemWidth();
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

        if (ImGui::CollapsingHeader("point light 1", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("light pos:");
            ImGui::SameLine();
            float pos[3] = {m_pointLight->position.x, m_pointLight->position.y, m_pointLight->position.z};
            ImGui::DragFloat3("##light pos", pos, 0.02f, -100.f, 100.f, "%.2f");
            m_pointLight->position = glm::vec3(pos[0], pos[1], pos[2]);
            ImGui::Checkbox("##enable point light", &m_pointLight->on);
            ImGui::SameLine();
            ImVec4 selectedColor = ImVec4(m_pointLight->color.x, m_pointLight->color.y, m_pointLight->color.z, 1.0f);
            ImGui::ColorButton("##point light color", selectedColor);
            ImGui::PushItemWidth(60.0f);
            ImGui::SameLine();
            ImGui::SliderFloat("##point light r", &m_pointLight->color.x, 0.0f, 1.0f, "%.2f");
            ImGui::SameLine();
            ImGui::SliderFloat("##point light g", &m_pointLight->color.y, 0.0f, 1.0f, "%.2f");
            ImGui::SameLine();
            ImGui::SliderFloat("##point light b", &m_pointLight->color.z, 0.0f, 1.0f, "%.2f");

            ImGui::Text("intensity:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(100.0f);
            if (ImGui::InputFloat("##point light intensity", &m_pointLight->intensity, 1.0f, 2.0f, "%.2f", ImGuiInputTextFlags_CharsDecimal))
            {
                if (m_pointLight->intensity < 1.0f)
                    m_pointLight->intensity = 1.0f;
            };
            ImGui::PopItemWidth();
        }
        ImGui::Unindent(DEFAULT_INDENT);
    }

    if (ImGui::CollapsingHeader("output", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Indent(DEFAULT_INDENT);
        ImGui::Text("Renderer: %s", glGetString(GL_RENDERER));
        ImGui::Text("frame: %d", frameCount);
        ImGui::Text("camera pos: %.2f %.2f %.2f", Camera::main_camera.pos().x, Camera::main_camera.pos().y, Camera::main_camera.pos().z);
        ImGui::Text("camera forward: %.2f %.2f %.2f", Camera::main_camera.forward().x, Camera::main_camera.forward().y, Camera::main_camera.forward().z);
        ImGui::Unindent(DEFAULT_INDENT);
    }

    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Engine::renderCookTorrancePBR()
{
    gLClearError();
    if (renderCTPBR)
    {
        Program *ct = m_programs.at("cook-torrance");
        ct->use();

        Model *ball = m_models.at("pool-ball");
        if (ball->loadFinished())
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), ball->getPosition());

            // vertex attributes
            ct->setUniformMatrix4fv("project", m_project);
            ct->setUniformMatrix4fv("view", Camera::main_camera.view());
            ct->setUniformMatrix4fv("model", model);

            // fragment attributes
            ct->setUniform3f("pointLight.position", m_pointLight->position);
            ct->setUniform3f("pointLight.color", m_pointLight->color);
            ct->setUniform1f("pointLight.intensity", m_pointLight->intensity);
            ct->setUniform3f("viewPos", Camera::main_camera.pos());
            ct->setUniform3f("albedo", Material::cCT_PBR.albedo);
            ct->setUniform1f("metallic", Material::cCT_PBR.metallic);
            ct->setUniform1f("roughness", Material::cCT_PBR.roughness);
            ct->setUniform1f("ao", Material::cCT_PBR.ao);
            ball->draw(ct);
        }
    }
    assert(gLCheckError());
}

#pragma endregion

void framebufferSizeCallback(GLFWwindow *pWindow, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouseCallback(GLFWwindow *pWindow, double x, double y)
{
    // prohibit using mouse in debugging mode
    if (!bDebugging)
    {
        g_mousePos = glm::vec2(x, y);

        static bool firstMouse = true;
        static double lastX = 0.f;
        static double lastY = 0.f;

        if (firstMouse)
        {
            lastX = x;
            lastY = y;
            firstMouse = false;
        }

        double xoffset = x - lastX;
        double yoffset = lastY - y;
        lastX = x;
        lastY = y;

        double sensitivity = 0.02f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;
        Camera::main_camera.updateAngle((float)xoffset, (float)yoffset);
    }
    else
    {
        g_mouseLastPos = glm::vec2(x, y);
        ImGuiIO &io = ImGui::GetIO();
        io.MousePos = ImVec2((float)x, (float)y);
    }
}

void processInput(GLFWwindow *pWindow, Camera *pCamera)
{
    if (nullptr == pWindow)
        return;

    if (glfwGetKey(pWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(pWindow, true);
    }

    if (glfwGetKey(pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS && !bDebugging)
    {
        bDebugging = true;
        changeMouseDisplay(pWindow, bDebugging);
        glfwSetCursorPos(pWindow, System::nScreenWidth / 2, System::nScreenHeight / 2);
    }

    if (pCamera == nullptr)
        return;

    if (!bDebugging)
    {
        if (glfwGetKey(pWindow, GLFW_KEY_W) == GLFW_PRESS)
            pCamera->move(Camera::FORWARD);
        if (glfwGetKey(pWindow, GLFW_KEY_S) == GLFW_PRESS)
            pCamera->move(Camera::BACKWARD);
        if (glfwGetKey(pWindow, GLFW_KEY_A) == GLFW_PRESS)
            pCamera->move(Camera::LEFT);
        if (glfwGetKey(pWindow, GLFW_KEY_D) == GLFW_PRESS)
            pCamera->move(Camera::RIGHT);
        if (glfwGetKey(pWindow, GLFW_KEY_Q) == GLFW_PRESS)
            pCamera->move(Camera::UP);
        if (glfwGetKey(pWindow, GLFW_KEY_E) == GLFW_PRESS)
            pCamera->move(Camera::DOWN);
    }
}

void changeMouseDisplay(GLFWwindow *pWindow, bool bDisplay)
{
    if (nullptr == pWindow)
        return;
    glfwSetInputMode(pWindow, GLFW_CURSOR, bDisplay ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

void changeMousePos(GLFWwindow *pWindow, glm::dvec2 mousePos)
{
    if (nullptr == pWindow)
        return;
    glfwSetCursorPos(pWindow, mousePos.x, mousePos.y);
}

void gLClearError()
{
    while (GL_NO_ERROR != glGetError());
}

bool gLCheckError()
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] " << error << std::endl;
        return false;
    }
    return true;
}

void generateCircleVertices(float *vertices, int width, int height)
{
    const float radius = 0.002f;
    vertices[0] = 0.f;
    vertices[1] = 0.f;

    for (int i = 0; i <= SEGMENTS; i++)
    {
        float angle = float(2.0f * M_PI * i / SEGMENTS);
        float aspectRatio = (float)width / (float)height;
        vertices[2 + i * 2] = cos(angle) * radius;
        vertices[3 + i * 2] = sin(angle) * radius * aspectRatio;
    }
}
