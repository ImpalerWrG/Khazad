#ifndef STDAFX__HEADER
#define STDAFX__HEADER

//#include <windows.h>
#include <stdio.h>
//#include <tchar.h>
#include <assert.h>

#include <GL/gl.h>
#include <GL/glu.h>
#ifdef LINUX_BUILD
    #include <GL/glut.h>
#else
    #include <glut.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>

#include <string>
#include <vector>
#include <math.h>
#include <algorithm>
#include <functional>
#include <iostream>
#include <stdarg.h>

#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include <guichan/opengl.hpp>
#include <guichan/opengl/openglsdlimageloader.hpp>


#include <Enum.h>

using namespace std;

#endif //STDAFX__HEADER
