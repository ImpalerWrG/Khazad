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

SDL_Surface* FontManager::makeFontSurface( const char* Text, SDL_Color Color, Uint8 FontIndex )
{
	SDL_Surface* FontSurface = NULL;
	if (FontIndex < FontLibrary.size())
	{
		TTF_Font* Font = FontLibrary[FontIndex];
		if (Font != NULL)
		{
			FontSurface = TTF_RenderText_Solid(Font, Text, Color);
		}
	}
	if (FontSurface)
	{
		return FontSurface;
	}
	return NULL;
}

void FontManager::RenderText(char *text, TTF_Font *font, SDL_Color color, SDL_Rect *location)
{
    SDL_Surface* FontSurface = TTF_RenderText_Blended(font, text, color);
    RenderSurface(FontSurface, location);
}

void FontManager::RenderSurface(SDL_Surface* RenderSurface, SDL_Rect *location)
{
	SDL_Surface *intermediary;
	SDL_Rect rect;
	GLuint texture;

	/* Convert the rendered text to a known format */
	rect.w = nextpoweroftwo(RenderSurface->w);
	rect.h = nextpoweroftwo(RenderSurface->h);

	intermediary = SDL_CreateRGBSurface(0, rect.w, rect.h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

	SDL_BlitSurface(RenderSurface, 0, intermediary, 0);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, rect.w, rect.h, 0, GL_BGRA, GL_UNSIGNED_BYTE, intermediary->pixels );

	/* GL_NEAREST looks horrible, if scaled... */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    RenderTexture(texture, &rect, location);

	/* return the deltas in the unused w,h part of the rect */
	location->w = RenderSurface->w;
	location->h = RenderSurface->h;

	SDL_FreeSurface(intermediary);
	glDeleteTextures(1, &texture);
}

void FontManager::RenderTexture(GLuint texture, SDL_Rect *Size, SDL_Rect *location)
{
 	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);   glVertex2f(location->x          , location->y);
		glTexCoord2f(1.0f, 1.0f);   glVertex2f(location->x + Size->w, location->y);
		glTexCoord2f(1.0f, 0.0f);   glVertex2f(location->x + Size->w, location->y + Size->h);
		glTexCoord2f(0.0f, 0.0f);   glVertex2f(location->x          , location->y + Size->h);
	glEnd();

	glFinish();
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
