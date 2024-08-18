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
#include <glm/glm.hpp>

class Program
{
public:
    Program();
    Program(const std::string& vertex_shader, const std::string& fragment_shader);
    ~Program();
    
    /// initial program
    void init();
    
    /// bind vertex shader on program
    /// - Parameter shader: vertex shader content
    void set_vertex_shader(const std::string& shader);
    
    /// bind framgent shader on program
    /// - Parameter shader: fragment shader content
    void set_fragment_shader(const std::string& shader);
    
    /// use this program
    void use();
    
    void set_uniform1i(const std::string& key, int value);
    void set_uniform3f(const std::string& key, const glm::vec3& value);
    void set_uniform3f(const std::string& key, float x, float y, float z);
    void set_uniformMatrix4fv(const std::string& key, const glm::mat4& value);
    
private:
    
    GLuint get_location(const std::string& param);
    
    std::string load_shader(const std::string& file_name);
    
    /// create shader and compile
    /// - Parameters:
    ///   - type: shader type
    ///   - shader: shader source
    unsigned int compile_shader(unsigned int type, const std::string& shader);
    
private:
    unsigned int m_program;
    
    std::map<std::string, unsigned int> m_mapUniformLoc;
};

#endif /* program_h */
