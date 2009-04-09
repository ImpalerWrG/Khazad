#include <stdafx.h>

#include <ImagePage.h>
#include <ClipImage.h>
#include <ImageManager.h>


ImageManager::ImageManager()
{

}

ImageManager::~ImageManager()
{

}

SDL_Surface* ImageManager::loadBMPSurface(char* filepath, bool Color)
{
	SDL_Surface* RawSurface = IMG_Load(filepath);
	SDL_Surface* ConvertedSurface = NULL;

	if(RawSurface != NULL)
	{
		ConvertedSurface = SDL_DisplayFormat(RawSurface);
		SDL_FreeSurface(RawSurface);
		if (ConvertedSurface != NULL)
		{
			if (Color)
			{
				Uint32 colorkey = SDL_MapRGB( ConvertedSurface->format, 0xFF, 0, 0xFF );
				SDL_SetColorKey( ConvertedSurface, SDL_SRCCOLORKEY, colorkey );
				return ConvertedSurface;
			}
			return ConvertedSurface;
		}
		return NULL;
	}
	return NULL;
}

SDL_Surface* ImageManager::loadSurface(char* filepath, bool Color)
{
	SDL_Surface* RawSurface = IMG_Load(filepath);
	SDL_Surface* ConvertedSurface = NULL;

	if(RawSurface != NULL)
	{
		if (Color)
		{
			ConvertedSurface = SDL_DisplayFormat(RawSurface);
			SDL_FreeSurface(RawSurface);
			if (ConvertedSurface != NULL)
			{
				if (Color)
				{
					Uint32 colorkey = SDL_MapRGB( ConvertedSurface->format, 0xFF, 0, 0xFF );
					SDL_SetColorKey( ConvertedSurface, SDL_SRCCOLORKEY, colorkey );
					return ConvertedSurface;
				}
				return ConvertedSurface;
			}
		}
		else
		{
			ConvertedSurface = SDL_DisplayFormatAlpha(RawSurface);
			SDL_FreeSurface(RawSurface);
			if (ConvertedSurface != NULL)
			{
				return ConvertedSurface;
			}
			return ConvertedSurface;
		}

		return NULL;
	}
	return NULL;
}

ClipImage* ImageManager::loadSingleSurface(char* filepath, bool ColorKey, bool bmp)
{
	SDL_Surface* Surface;
	if (bmp)
	{
		Surface = loadBMPSurface(filepath, ColorKey);
	}
	else
	{
		Surface = loadSurface(filepath, ColorKey);
	}

	ImagePage* NewPage = new ImagePage(Surface, Surface->h, Surface->w, 1, 1);
	ImageLibrary.push_back(NewPage);

	ClipImage* NewClip = new ClipImage(NewPage);
	ClipLibrary.push_back(NewClip);

	return NewClip;
}

void ImageManager::loadClippedSurface(char* filepath, int cliphight, int clipwidth, int rows, int columns, bool ColorKey, bool bmp)
{
	SDL_Surface* Surface;
	if (bmp)
	{
		Surface = loadBMPSurface(filepath, ColorKey);
	}
	else
	{
		Surface = loadSurface(filepath, ColorKey);
	}

	ImagePage* NewPage = new ImagePage(Surface, cliphight, clipwidth, rows, columns);
	ImageLibrary.push_back(NewPage);

	for(int i = 0; i < rows; i++)
	{
		for(int j = 0; j < columns; j++)
		{
			ClipImage* NewClip = new ClipImage(NewPage, i, j);
			ClipLibrary.push_back(NewClip);
		}
	}
}
