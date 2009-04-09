#ifndef COLOR__HEADER
#define COLOR__HEADER

#include <stdafx.h>

#include <Singleton.h>

static const SDL_Color WHITE = {255, 255, 255};
static const SDL_Color BLACK = {0, 0, 0};

static const SDL_Color RED = {255, 0, 0};
static const SDL_Color BLUE = {0, 0, 255};
static const SDL_Color GREEN = {0, 255, 0};
static const SDL_Color YELLOW = {255, 255, 0};


class ColorManager
{
DECLARE_SINGLETON_CLASS(ColorManager)

public:

	~ColorManager();
	bool Init();

	std::vector<SDL_Color*> ColorLibrary;

};

#define COLOR (ColorManager::GetInstance())

#endif  // COLOR__HEADER

