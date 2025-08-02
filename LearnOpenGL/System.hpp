//
//  System.h
//  LearnOpenGL
//
//  Created by asi on 2024/8/9.
//

#ifndef System_h
#define System_h

#include <string>

#ifdef _WIN32
#include <filesystem>
#elif defined(__APPLE__)
#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>
#endif

namespace System
{

static int nScreenWidth = 1920;
static int nScreenHeight = 1080;

inline std::string resourcePathWithFile(const std::string& file)
{
    std::string strPath = "";
#ifdef _WIN32
    std::filesystem::path currentPath = std::filesystem::current_path();
    //strPath = currentPath.string() + std::string("\\Resources\\") + file;
    strPath = "E:\\Code\\OpenGL\\LearnOpenGL\\Resources\\" + file;
#elif defined(__APPLE__)
    NSString* path = [[NSBundle mainBundle]resourcePath];
    strPath = [path UTF8String];
    strPath = strPath + "/" + file;
#endif
    return strPath;
}

}
#endif /* System_hpp */
