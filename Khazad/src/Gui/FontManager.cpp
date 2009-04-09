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

void FontManager::SDL_GL_RenderText(char *text, TTF_Font *font, SDL_Color color, SDL_Rect *location)
{
    int TextureID = 1;

	SDL_Surface *initial;
	SDL_Surface *intermediary;
	SDL_Rect rect;
	int w, h;
	GLuint texture;

	/* Use SDL_TTF to render our text */
	initial = TTF_RenderText_Blended(font, text, color);

	/* Convert the rendered text to a known format */
	w = nextpoweroftwo(initial->w);
	h = nextpoweroftwo(initial->h);

	intermediary = SDL_CreateRGBSurface(0, w, h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

	SDL_BlitSurface(initial, 0, intermediary, 0);

	/* Tell GL about our new texture */
	glGenTextures(TextureID, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, intermediary->pixels );

	/* GL_NEAREST looks horrible, if scaled... */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/* prepare to render our texture */
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glColor3f(1.0f, 1.0f, 1.0f);

	/* Draw a quad at location */
	glBegin(GL_QUADS);
		/* Recall that the origin is in the lower-left corner
		   That is why the TexCoords specify different corners
		   than the Vertex coors seem to. */
		glTexCoord2f(0.0f, 1.0f);
			glVertex2f(location->x    , location->y);
		glTexCoord2f(1.0f, 1.0f);
			glVertex2f(location->x + w, location->y);
		glTexCoord2f(1.0f, 0.0f);
			glVertex2f(location->x + w, location->y + h);
		glTexCoord2f(0.0f, 0.0f);
			glVertex2f(location->x    , location->y + h);
	glEnd();

	/* Bad things happen if we delete the texture before it finishes */
	glFinish();

	/* return the deltas in the unused w,h part of the rect */
	location->w = initial->w;
	location->h = initial->h;

	/* Clean up */
	SDL_FreeSurface(initial);
	SDL_FreeSurface(intermediary);
	glDeleteTextures(TextureID, &texture);
}

int FontManager::round(double x)
{
	return (int)(x + 0.5);
}

int FontManager::nextpoweroftwo(int x)
{
	double logbase2 = log(x) / log(2);
	return round(pow(2,ceil(logbase2)));
}
