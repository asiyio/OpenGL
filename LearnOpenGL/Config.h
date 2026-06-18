//
//  config.h
//  LearnOpenGL
//
//  Created by Rocky on 2024/7/28.
//

#ifndef config_h
#define config_h

#include <glm/glm.hpp>

// 定义展示在imgui上的配置
bool renderSkyBox = true;
bool renderCTPBR = true;
bool renderSSAO = false;
int frameCount = 0;
float ssaoRadius = 0.5f;    // ssao采样半径

#endif /* config_h */
