#include <stdafx.h>

#include <ImagePage.h>
#include <ClipImage.h>
#include <TextureManager.h>


DECLARE_SINGLETON(TextureManager)

bool TextureManager::Init()
{
	return true;
}

TextureManager::TextureManager()
{

}

TextureManager::~TextureManager()
{

}

SDL_Surface* TextureManager::loadBMPSurface(char* filepath, bool Color)
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

SDL_Surface* TextureManager::loadSurface(char* filepath, bool Color)
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

ClipImage* TextureManager::loadSingleSurface(char* filepath, bool ColorKey, bool bmp)
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

	if (Surface)
	{
		ImagePage* NewPage = new ImagePage(Surface, Surface->h, Surface->w, 1, 1);
		ImageLibrary.push_back(NewPage);

		ClipImage* NewClip = new ClipImage(NewPage);
		ClipLibrary.push_back(NewClip);

		return NewClip;
	}
	return NULL;
}

void TextureManager::loadClippedSurface(char* filepath, int cliphight, int clipwidth, int rows, int columns, bool ColorKey, bool bmp)
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

void TextureManager::loadTextureSingular(char* filepath, bool ColorKey, bool bmp)
{
    SDL_Surface* TextureImage = NULL;
	TextureImage = IMG_Load(filepath);

/*
	if (ColorKey)
	{
		SDL_SetColorKey(TextureImage, SDL_SRCCOLORKEY, SDL_MapRGB(TextureImage->format, 255, 0, 255));
		SDL_Surface* alphasurface = SDL_DisplayFormatAlpha(TextureImage);

		TextureImage = alphasurface;
		SDL_FreeSurface(alphasurface);
	}
	*/


	//NewClip = loadSingleSurface(filepath, ColorKey, bmp);

	//TextureImage = NewClip->ParentPage->RawSurface;


    if ( TextureImage )
	{
		GLuint NewTex;

	    glGenTextures( 1, &NewTex );
	    glBindTexture( GL_TEXTURE_2D, NewTex );

		SingularTextureLibrary.push_back(NewTex);

		if (bmp)
		{
			glTexImage2D( GL_TEXTURE_2D, 0, 3, TextureImage->w, TextureImage->h, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, TextureImage->pixels );
		}
		else
		{
			glTexImage2D( GL_TEXTURE_2D, 0, 3, TextureImage->w, TextureImage->h, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage->pixels );
		}

	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	}

    if ( TextureImage )
	{
	    SDL_FreeSurface( TextureImage );
	}
}
