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
	SDL_Surface* makeFontSurface( const char* Text, SDL_Color Color, Uint8 FontIndex = 0 );

    void RenderText(char *text, TTF_Font *font, SDL_Color color, SDL_Rect *location);
    void RenderSurface(SDL_Surface* RenderSurface, SDL_Rect *location);
    void RenderTexture(GLuint texture, SDL_Rect *Size, SDL_Rect *location);

    void glEnable2D();
    void glDisable2D();

    int round(double x);
    int nextpoweroftwo(int x);


protected:

    gcn::ImageFont* font;

	TTF_Font* DEFAULTFONT;
};

#define FONT (FontManager::GetInstance())

#endif // FONT__HEADER
