#include <stdafx.h>

#include <ImagePage.h>


ImagePage::ImagePage(SDL_Surface* Surface, int arghight, int argwidth, int argRows, int argColumns)
{
	RawSurface = Surface;

	clipHight = arghight;
	clipWidth = argwidth;

	Rows = argRows;
	Columns = argColumns;
}

ImagePage::~ImagePage()
{
	SDL_FreeSurface(RawSurface);
}
