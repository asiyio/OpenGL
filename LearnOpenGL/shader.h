//
//  shader.h
//  LearnOpenGL
//
//  Created by Rocky on 2024/7/28.
//

#ifndef shader_h
#define shader_h

#include <string>

std::string vertexShader =
"#version 330 core\n"
"layout(location = 0) in vec4 position;\n"
"void main(){\n"
"   gl_Position = position;\n"
"}\n"
;

std::string fragmentShader =
"#version 330 core\n"
"out vec4 FragColor;\n"
"void main(){\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n"
;

#endif /* shader_h */
