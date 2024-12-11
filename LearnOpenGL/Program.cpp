//
//  program.cpp
//  LearnOpenGL
//
//  Created by asi on 2024/8/4.
//

#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include "Program.h"
#include "System.h"


Program::Program()
    : m_program(0)
{

}

Program::Program(const std::string& vertex_shader, const std::string& fragment_shader)
    : m_program(0)
{
    init();
    set_vertex_shader(vertex_shader);
    set_fragment_shader(fragment_shader);
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
    if (linkStatus != GL_TRUE)
    {
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
    if (linkStatus != GL_TRUE)
    {
        GLchar infoLog[512];
        glGetProgramInfoLog(m_program, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Program linking failed:\n" << infoLog << std::endl;
    }
}

void Program::set_geometry_shader(const std::string& shader)
{
    std::string strGeometryShader = load_shader(shader);
    unsigned int nGeometryShader = compile_shader(GL_GEOMETRY_SHADER, strGeometryShader);
    glAttachShader(m_program, nGeometryShader);
    glLinkProgram(m_program);
    glDeleteShader(nGeometryShader);
    
    GLint linkStatus;
    glGetProgramiv(m_program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE)
    {
        GLchar infoLog[512];
        glGetProgramInfoLog(m_program, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Program linking failed:\n" << infoLog << std::endl;
    }
}

void Program::use()
{
    glLinkProgram(m_program);
    
    GLint linkStatus;
    glGetProgramiv(m_program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<char> infoLog(maxLength);
        glGetProgramInfoLog(m_program, maxLength, &maxLength, &infoLog[0]);

        std::cerr << "shader program linking failed:\n" << &infoLog[0] << "\n";
    } else {
        std::cout << "shader program linked successfully\n";
    }
    
    if (glIsProgram(m_program)) {
        std::cout << "shader program is valid\n";
    } else {
        std::cerr << "shader program is invalid\n";
    }
    
    glUseProgram(m_program);
    
    GLenum error = glGetError();
    if (error == GL_NO_ERROR) {
        GLint currentProgram;
        glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);

        if (currentProgram == m_program) {
            std::cout << "shader program is active and in use\n";
        } else {
            std::cerr << "shader program binding failed\n";
        }
    } else {
        std::cerr << "glUseProgram failed with error: " << error << "\n";
    }
}

void Program::set_uniform1i(const std::string& key, int value)
{
    GLuint location = get_location(key);
    if (location >= 0)
    {
        glUniform1i(location, value);
    }
}

void Program::set_uniform1f(const std::string& key, float value)
{
    GLuint location = get_location(key);
    if (location >= 0)
    {
        glUniform1f(location, value);
    }
}

void Program::set_uniform3f(const std::string& key, const glm::vec3& value)
{
    GLuint location = get_location(key);
    if (location >= 0)
    {
        glUniform3f(location, value.x, value.y, value.z);
    }
}

void Program::set_uniform3f(const std::string& key, float x, float y, float z)
{
    GLuint location = get_location(key);
    if (location >= 0)
    {
        glUniform3f(location, x, y, z);
    }
}

void Program::set_uniformMatrix4fv(const std::string& key, const glm::mat4& value)
{
    GLuint location = get_location(key);
    if (location >= 0)
    {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }
}

void Program::set_uniformSpotLights(const std::vector<SpotLight*>& spotLights)
{
    std::string key = "";
    set_uniform1i("num_spot_lights", (int)spotLights.size());
    for (int i = 0; i < spotLights.size(); i++) {
        key = "spot_lights[" + std::to_string(i) + "].position";
        set_uniform3f(key, spotLights[i]->position);
        
        key = "spot_lights[" + std::to_string(i) + "].color";
        set_uniform3f(key, spotLights[i]->color);
        
        key = "spot_lights[" + std::to_string(i) + "].constant";
        set_uniform1f(key, spotLights[i]->constant);
        
        key = "spot_lights[" + std::to_string(i) + "].linear";
        set_uniform1f(key, spotLights[i]->linear);
        
        key = "spot_lights[" + std::to_string(i) + "].quadratic";
        set_uniform1f(key, spotLights[i]->quadratic);
        
        key = "spot_lights[" + std::to_string(i) + "].on";
        set_uniform1i(key, spotLights[i]->on);
    }
}

void Program::set_uniformFlashLight(const std::vector<FlashLight*>& flashLights)
{
    std::string key = "";
    set_uniform1i("num_flash_lights", (int)flashLights.size());
    for (int i = 0; i < flashLights.size(); i++) {
        key = "flash_lights[" + std::to_string(i) + "].position";
        set_uniform3f(key, flashLights[i]->position);
        
        key = "flash_lights[" + std::to_string(i) + "].direction";
        set_uniform3f(key, flashLights[i]->direction);
        
        key = "flash_lights[" + std::to_string(i) + "].color";
        set_uniform3f(key, flashLights[i]->color * flashLights[i]->intensity);
        
        key = "flash_lights[" + std::to_string(i) + "].cutOff";
        set_uniform1f(key, flashLights[i]->cutOff);
        
        key = "flash_lights[" + std::to_string(i) + "].outerCutOff";
        set_uniform1f(key, flashLights[i]->outerCutOff);
        
        key = "flash_lights[" + std::to_string(i) + "].constant";
        set_uniform1f(key, flashLights[i]->constant);
        
        key = "flash_lights[" + std::to_string(i) + "].linear";
        set_uniform1f(key, flashLights[i]->linear);
        
        key = "flash_lights[" + std::to_string(i) + "].quadratic";
        set_uniform1f(key, flashLights[i]->quadratic);
        
        key = "flash_lights[" + std::to_string(i) + "].on";
        set_uniform1i(key, flashLights[i]->on);
    }
}

GLuint Program::get_location(const std::string& param)
{
    GLuint location = -1;
    if (m_mapUniformLoc.find(param) != m_mapUniformLoc.end())
    {
        location = m_mapUniformLoc.at(param);
    }
    else
    {
        location = glGetUniformLocation(m_program, param.c_str());
        if (location >= 0)
        {
            m_mapUniformLoc[param] = location;
        }
    }
    return location;
}

std::string Program::load_shader(const std::string &file_name)
{
    std::string path = System::ResourcePathWithFile(file_name);
    std::ifstream file(path);

    if (!file)
    {
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
