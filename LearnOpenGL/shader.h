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
"layout(location = 0) in vec3 aPosition;\n"
"layout(location = 1) in vec3 aColor;\n"
"out vec3 color;\n"
"uniform float time;\n"
"void main(){\n"
"   vec4 position = vec4(aPosition, 1.0);\n"
"   //position.xy = position.xy * 2 - 1.0;\n"
"   gl_Position = position;\n"
"   color = aColor * (sin(time) * 0.5f + 0.5f);\n"
"}\n"
;

std::string fragmentShader =
"#version 330 core\n"
"in vec3 color;\n"
"out vec4 FragColor;\n"
"void main(){\n"
"   FragColor = vec4(color, 1.0f);\n"
"}\n"
;

#endif /* shader_h */
