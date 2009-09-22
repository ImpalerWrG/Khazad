#include <stdafx.h>

#include <ImagePage.h>
#include <ClipImage.h>
#include <ImageManager.h>


DECLARE_SINGLETON(ImageManager)

ImageManager::ImageManager()
{

}

ImageManager::~ImageManager()
{

}

bool ImageManager::Init()
{
    return true;
}

ILuint ImageManager::loadImage(char* filepath, bool ColorKey)
{
    ILuint ImageID;
    ilGenImages(1, &ImageID);
    ilBindImage(ImageID);

    printf("Loading Image file: %s\n", filepath);
    ilLoadImage(filepath);

    /*
    IL_RGB
    IL_RGBA
    IL_BGR
    IL_BGRA
    IL_LUMINANCE
    IL_COLOUR_INDEX


    IL_BYTE
    IL_UNSIGNED_BYTE
    IL_SHORT
    IL_UNSIGNED_SHORT
    IL_INT
    IL_UNSIGNED_INT
    IL_FLOAT
    IL_DOUBLE
    */


    ilConvertImage(IL_BGRA, IL_UNSIGNED_BYTE);

    if(ColorKey)
    {
        //convert color key
    }

    ReportDevILErrors();
    return ImageID;
}

ClipImage* ImageManager::loadSingleSurface(char* filepath, bool ColorKey)
{
	ILuint ImageID;
    ImageID = loadImage(filepath, ColorKey);

	ImagePage* NewPage = new ImagePage(NULL, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 1, 1);
	ImageLibrary.push_back(NewPage);

	ClipImage* NewClip = new ClipImage(NewPage);
	ClipLibrary.push_back(NewClip);

	return NewClip;
}

void ImageManager::loadClippedSurface(char* filepath, int cliphight, int clipwidth, int rows, int columns, bool ColorKey)
{
    /*
	SDL_Surface* Surface;
    Surface = loadSurface(filepath, ColorKey);

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
	*/
}

SDL_Surface* ImageManager::loadSurface(char* filepath, bool ColorKey)
{
	SDL_Surface* RawSurface = IMG_Load(filepath);
	SDL_Surface* ConvertedSurface = NULL;

	if(RawSurface != NULL)
	{
		if (ColorKey)
		{
			ConvertedSurface = SDL_DisplayFormat(RawSurface);
			SDL_FreeSurface(RawSurface);
			if (ConvertedSurface != NULL)
			{
				if (ColorKey)
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
			if (ConvertedSurface != NULL)
			{
                SDL_FreeSurface(RawSurface);
				return ConvertedSurface;
			}
            SDL_FreeSurface(ConvertedSurface);
			return RawSurface;
		}

		return NULL;
	}
	return NULL;
}

void ImageManager::ReportDevILErrors()
{
    ILenum Error;
    while ((Error = ilGetError()) != IL_NO_ERROR)
    {
        printf("DevIL Error %d: %s\n", Error, iluErrorString(Error));
    }
}
