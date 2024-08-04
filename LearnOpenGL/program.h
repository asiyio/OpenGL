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

class Program
{
public:
    Program();
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
    
    unsigned int get_location(const std::string& param);
    
private:
    
    std::string load_shader(const std::string& file_name);
    
    /// create shader and compile
    /// - Parameters:
    ///   - type: shader type
    ///   - shader: shader source
    unsigned int compile_shader(unsigned int type, const std::string& shader);
    
private:
    unsigned int m_program;
};

#endif /* program_h */
