//
//  System.mm
//  LearnOpenGL
//
//  Created by asi on 2024/8/9.
//

#include "System.h"
#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>

namespace System
{

void GetScreenSize(unsigned int* nWidth, unsigned int* nHeight)
{
    NSScreen *screen = [NSScreen mainScreen];
    NSRect screenRect = [screen frame];
    *nWidth = (unsigned int)screenRect.size.width;
    *nHeight = (unsigned int)screenRect.size.height;
}

std::string ResourcePathWithFile(const std::string& file)
{
    NSString* path = [[NSBundle mainBundle] resourcePath];
    
    std::string strPath = [path UTF8String];
    strPath = strPath + "/" + file;
    return strPath;
}

}


