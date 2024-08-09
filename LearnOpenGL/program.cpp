//
//  program.cpp
//  LearnOpenGL
//
//  Created by asi on 2024/8/4.
//

#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "program.h"
#include "ResourcePath.h"


Program::Program()
    : m_program(0)
{

}

Program::~Program()
{
    glDeleteProgram(m_program);
}

void Program::init()
{
    m_program = glCreateProgram();
}

void Program::set_vertex_shader(const std::string& shader)
{
    std::string strVertexShader = load_shader(shader);
    unsigned int nVertexShader = compile_shader(GL_VERTEX_SHADER, strVertexShader);
    glAttachShader(m_program, nVertexShader);
    glLinkProgram(m_program);
    glDeleteShader(nVertexShader);
    
    GLint linkStatus;
    glGetProgramiv(m_program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE) {
        GLchar infoLog[512];
        glGetProgramInfoLog(m_program, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Program linking failed:\n" << infoLog << std::endl;
    }
}

void Program::set_fragment_shader(const std::string& shader)
{
    std::string strFragmentShader = load_shader(shader);
    unsigned int nFragmentShader = compile_shader(GL_FRAGMENT_SHADER, strFragmentShader);
    glAttachShader(m_program, nFragmentShader);
    glLinkProgram(m_program);
    glDeleteShader(nFragmentShader);
    
    GLint linkStatus;
    glGetProgramiv(m_program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE) {
        GLchar infoLog[512];
        glGetProgramInfoLog(m_program, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Program linking failed:\n" << infoLog << std::endl;
    }
}

void Program::use()
{
    glUseProgram(m_program);
}

unsigned int Program::get_location(const std::string& param)
{
    return glGetUniformLocation(m_program, param.c_str());
}

std::string Program::load_shader(const std::string &file_name)
{
    std::string path = ResourcePathWithFile(file_name);
    std::ifstream file(path);

    if (!file) {
        std::cerr << "Failed to open " << file_name << "!" << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    return buffer.str();
}

unsigned int Program::compile_shader(unsigned int type, const std::string& shader)
{
    unsigned int id = glCreateShader(type);
    const char* scr = shader.c_str();
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
        std::cerr << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader: " << message << std::endl;
        glDeleteShader(id);
        return 0;
    }
    return id;
}
