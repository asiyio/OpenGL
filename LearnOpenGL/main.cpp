//
//  main.cpp
//  LearnOpenGL
//
//  Created by Rocky on 2024/7/27.
//
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <cassert>

#include "shader.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

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

/// create shader and compile
/// - Parameters:
///   - nType: shader type
///   - strShader: shader source
static unsigned int compileShader(unsigned int nType, const std::string& strShader)
{
    unsigned int id = glCreateShader(nType);
    const char* scr = strShader.c_str();
    glShaderSource(id, 1, &scr, nullptr);
    glCompileShader(id);
    
    int nResult;
    glGetShaderiv(id, GL_COMPILE_STATUS, &nResult);
    if (nResult == GL_FALSE)
    {
        int nLength;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &nLength);
        char* message = (char*)alloca(nLength * sizeof(char));
        glGetShaderInfoLog(id, nLength, &nLength, message);
        std::cout << "Failed to compile " << (nType == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader: " << message << std::endl;
        glDeleteShader(id);
        return 0;
    }
    return id;
}

/// create a program and bind shader
/// - Parameters:
///   - strVertexShader: vertex shader source
///   - strFragmentShader: fragment shader sourse
static unsigned int createShader(const std::string& strVertexShader, const std::string& strFragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int nVertexShader = compileShader(GL_VERTEX_SHADER, strVertexShader);
    unsigned int nFragmentShader = compileShader(GL_FRAGMENT_SHADER, strFragmentShader);
    
    glAttachShader(program, nVertexShader);
    glAttachShader(program, nFragmentShader);
    glLinkProgram(program);
    
    glDeleteShader(nVertexShader);
    glDeleteShader(nFragmentShader);
    return program;
}

float vertices[] = {
    -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
     0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
};

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
    
    unsigned int program = createShader(vertexShader, fragmentShader);
    unsigned int timeLocation = glGetUniformLocation(program, "time");
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float time = glfwGetTime();
        glUniform1f(timeLocation, time);
        
        GLClearError();
        
        glUseProgram(program);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        assert(GLCheckError());
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
