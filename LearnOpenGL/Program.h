//
//  program.h
//  LearnOpenGL
//
//  Created by asi on 2024/8/4.
//

#ifndef program_h
#define program_h

#include <iostream>
#include <string>
#include <map>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Light.hpp"

class Program
{
public:
    Program();
    Program(const std::string& vertex_shader, const std::string& fragment_shader);
    ~Program();
    
    /// initial program
    void init();
    
    /// bind shader for program
    void setVertexShader(const std::string& shader);
    void setFragmentShader(const std::string& shader);
    void setGeometryShader(const std::string& shader);

    /// use the program
    void use();
    
    /// set shader uniform value
    void setUniform1i(const std::string& key, int value);
    void setUniform1f(const std::string& key, float value);
    void setUniform3f(const std::string& key, const glm::vec3& value);
    void setUniform3f(const std::string& key, float x, float y, float z);
    void setUniformMatrix4fv(const std::string& key, const glm::mat4& value);
    void setUniformSpotLights(const std::vector<SpotLight*>& spotLights);
    void setUniformFlashLight(const std::vector<FlashLight*>& flashLights);
    
private:
    
    GLuint getLocation(const std::string& param);
    
    std::string loadShader(const std::string& file_name);
    
    /// create shader and compile
    /// - Parameters:
    ///   - type: shader type
    ///   - shader: shader source
    unsigned int compileShader(unsigned int type, const std::string& shader);
    
private:
    unsigned int m_program;
    
    std::map<std::string, unsigned int> m_mapUniformLoc;
};

#endif /* program_h */
