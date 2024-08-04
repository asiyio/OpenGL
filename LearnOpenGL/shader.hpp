//
//  Shader.hpp
//  LearnOpenGL
//
//  Created by asi on 2024/8/4.
//

#ifndef Shader_h
#define Shader_h

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

namespace Shader
{
    std::string load_shader(const std::string& filename)
    {
        std::ifstream file(filename);

        if (!file) {
            std::cout << "fail to open " << filename << "!" << std::endl;
            return "";
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        
        return buffer.str();
    }
}

#endif /* Shader_h */
