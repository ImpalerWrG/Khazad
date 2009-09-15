#ifndef FONT__HEADER
#define FONT__HEADER

#include <SDL_ttf.h>
#include <stdafx.h>
#include <Singleton.h>

class FontManager
{

DECLARE_SINGLETON_CLASS(FontManager)

public:

	bool Init();
	~FontManager();

	std::vector<TTF_Font*> FontLibrary;

	TTF_Font* loadFont(char* filepath, Uint32 size);
	SDL_Surface* makeFontSurface( const char* Text, SDL_Color Color, Sint8 FontIndex = -1);

    TTF_Font* getDefaultFont();

    int round(double x);
    int nextpoweroftwo(int x);


protected:

    gcn::ImageFont* font;

	TTF_Font* DEFAULTFONT;
};

#define FONT (FontManager::GetInstance())

#endif // FONT__HEADER
