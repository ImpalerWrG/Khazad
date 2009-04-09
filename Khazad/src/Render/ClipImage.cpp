#include <stdafx.h>

#include <ClipImage.h>
#include <ImagePage.h>


ClipImage::ClipImage(ImagePage* Page, int row, int column)
{
	wholeImage = false;
	ParentPage = Page;

	OffsetRect.x = column * ParentPage->clipWidth;
	OffsetRect.y = row * ParentPage->clipHight;

	OffsetRect.h = ParentPage->clipHight;
	OffsetRect.w = ParentPage->clipWidth;

	Row = row;
	Column = column;
}

ClipImage::ClipImage(ImagePage* Page)
{
	wholeImage = true;
	ParentPage = Page;
}

ClipImage::~ClipImage()
{

}
