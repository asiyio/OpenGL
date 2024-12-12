//
//  System.h
//  LearnOpenGL
//
//  Created by asi on 2024/8/9.
//

#ifndef System_h
#define System_h

#include <string>

namespace System
{

static int nScreenWidth = 1200;
static int nScreenHeight = 800;

void getScreenSize(int* nWidth, int* nHeight);
std::string resourcePathWithFile(const std::string& file);

}

#endif /* System_hpp */
