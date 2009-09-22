#ifndef CLIPIMAGE__HEADER
#define CLIPIMAGE__HEADER

#include <stdafx.h>

class ImagePage;

class ClipImage
{
public:

	ClipImage(ImagePage* Page, int row, int column);
	ClipImage(ImagePage* Page);
	~ClipImage();

	bool wholeImage;

	SDL_Rect SizeRect;

	int Row;
	int Column;

	ImagePage* ParentPage;
	SDL_Rect OffsetRect;
};

#endif // CLIPIMAGE__HEADER

