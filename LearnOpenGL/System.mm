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

void getScreenSize(int* nWidth, int* nHeight)
{
    NSScreen *screen = [NSScreen mainScreen];
    NSRect screenRect = [screen frame];
    *nWidth = screenRect.size.width;
    *nHeight = screenRect.size.height;
}

std::string resourcePathWithFile(const std::string& file)
{
    NSString* path = [[NSBundle mainBundle] resourcePath];
    
    std::string strPath = [path UTF8String];
    strPath = strPath + "/" + file;
    return strPath;
}

}


