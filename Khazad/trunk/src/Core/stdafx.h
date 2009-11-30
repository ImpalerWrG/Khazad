#ifndef STDAFX__HEADER
#define STDAFX__HEADER

//#include <windows.h>
#include <stdio.h>
//#include <tchar.h>
#include <assert.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL.h>

#ifdef LINUX_BUILD
    #include <GL/glext.h>
#else
    #include <SDL_opengl.h>
#endif

#include <string>
#include <vector>
#include <math.h>
#include <algorithm>
#include <functional>
#include <iostream>
#include <stdarg.h>
#include <stdint.h>

#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include <guichan/opengl.hpp>
#include <guichan/opengl/openglsdlimageloader.hpp>


#include <Enum.h>

using namespace std;

#endif //STDAFX__HEADER
