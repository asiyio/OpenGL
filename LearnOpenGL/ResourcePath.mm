//
//  ResourcePath.cpp
//  LearnOpenGL
//
//  Created by asi on 2024/8/9.
//

#include "ResourcePath.h"
#import <Foundation/Foundation.h>

std::string ResourcePathWithFile(const std::string& file)
{
    NSString* path = [[NSBundle mainBundle] resourcePath];
    
    std::string strPath = [path UTF8String];
    strPath = strPath + "/" + file;
    return strPath;
}
