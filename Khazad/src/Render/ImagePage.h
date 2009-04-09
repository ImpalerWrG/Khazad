#ifndef IMAGE__HEADER
#define IMAGE__HEADER

#include <stdafx.h>
#include <SDL_image.h>

class ImagePage
{
public:

	ImagePage(SDL_Surface* Surface, int arghight, int argwidth, int argRows, int argColumns);
	~ImagePage();

	SDL_Surface* RawSurface;
	
	int clipHight;
	int clipWidth;

	int Rows;
	int Columns;
};

#endif // IMAGE__HEADER
