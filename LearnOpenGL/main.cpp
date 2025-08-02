//
//  main.cpp
//  LearnOpenGL
//
//  Created by Rocky on 2024/7/27.
//

#ifdef _WIN32
#include <Windows.h>
#include "Engine.h"
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
#else
#include "Engine.h"
int main(int argc, char* argv[])
#endif
{
    Engine::engine.start();
    return 0;
}
