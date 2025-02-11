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
#include <windows.h>
#elif defined(__APPLE__)
#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>
#endif

namespace System
{

static int nScreenWidth = 1920;
static int nScreenHeight = 1080;

inline void getScreenSize(int* nWidth, int* nHeight)
{

#ifdef _WIN32
	*nWidth = GetSystemMetrics(SM_CXSCREEN);
	*nHeight = GetSystemMetrics(SM_CYSCREEN);
#elif defined(__APPLE__)
    NSScreen* screen = [NSScreen mainScreen];
    NSRect screenRect = [screen frame];
    *nWidth = screenRect.size.width;
    *nHeight = screenRect.size.height;
#endif
    
}

inline std::string resourcePathWithFile(const std::string& file)
{
    std::string strPath = "";
#ifdef _WIN32
	char path[MAX_PATH];
	GetModuleFileNameA(NULL, path, MAX_PATH);
	std::string exePath = std::string(path);
	strPath = exePath.substr(0, exePath.find_last_of("\\/")) + "\\Resources\\" + file;
#elif defined(__APPLE__)
    NSString* path = [[NSBundle mainBundle]resourcePath];
    strPath = [path UTF8String];
    strPath = strPath + "/" + file;
#endif
    return strPath;
}

}

#endif /* System_hpp */
