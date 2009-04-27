#include <stdafx.h>

#include <SDL_ttf.h>
#include <FontManager.h>
#include <Singleton.h>

DECLARE_SINGLETON(FontManager)


bool FontManager::Init()
{
	if( TTF_Init() == -1 )
	{
		return false;
	}

    font = new gcn::ImageFont("Assets\\Fonts\\fixedfont.bmp", " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
    gcn::Widget::setGlobalFont(font);

	return true;
}

FontManager::FontManager()
{

}

FontManager::~FontManager()
{
	for(Uint32 i = 0; i < FontLibrary.size(); i++)
	{
		TTF_CloseFont(FontLibrary[i]);
	}
}

TTF_Font* FontManager::loadFont(char* filepath, Uint32 size)
{
	TTF_Font* newFont;
	newFont = TTF_OpenFont( filepath, size );
	FontLibrary.push_back(newFont);
	return newFont;
}

SDL_Surface* FontManager::makeFontSurface(const char* Text, SDL_Color Color, Sint8 FontIndex)
{
    TTF_Font* Font = NULL;
	SDL_Surface* FontSurface = NULL;

    if(FontIndex < 0)
    {
        Font = getDefaultFont();
    }
    else
    {
        if (FontIndex < FontLibrary.size())
        {
            Font = FontLibrary[FontIndex];
        }
    }

    if (Font != NULL)
    {
        FontSurface = TTF_RenderText_Solid(Font, Text, Color);
    }

    return FontSurface;
}

TTF_Font* FontManager::getDefaultFont()
{
    return FontLibrary[0];
}

int FontManager::round(double x)
{
	return (int)(x + 0.5);
}

int FontManager::nextpoweroftwo(int x)
{
	double logbase2 = log(x) / log(2);
	return round(pow(2, ceil(logbase2)));
}
