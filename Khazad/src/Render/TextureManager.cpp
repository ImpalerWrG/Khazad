#include <stdafx.h>

#include <ImagePage.h>
#include <ClipImage.h>
#include <TextureManager.h>
#include <DataManager.h>


DECLARE_SINGLETON(TextureManager)

bool TextureManager::Init()
{
    for(int i = 0; i < DATA->getNumTextures(); ++i)
    {
        loadTextureSingular(DATA->getTextureData(i)->getPath(), false, false);
    }
    MergeTextures();

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

SDL_Surface* TextureManager::loadTextureSingular(char* filepath, bool ColorKey, bool bmp)
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


    if (TextureImage)
	{
        RawTextureVector.push_back(TextureImage);
        return TextureImage;
	}
	return NULL;
}

void TextureManager::MergeTextures()
{
    Uint32 LargestTextureSize = 256;  // TODO Must be found dynamicly, see bellow
    float root = sqrt((float) RawTextureVector.size());
    MainTextureSize = nextpoweroftwo(round(root)) * LargestTextureSize;

    AgragateSurface = SDL_CreateRGBSurface(0, MainTextureSize, MainTextureSize, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);

    SDL_Surface* Source;
    Uint32 HorizonalSpace;

    SDL_Rect Destination;
    Destination.x = 0;
    Destination.y = 0;
    SDL_Rect TextureCorners;

    for(Uint32 i = 0; i < RawTextureVector.size(); i++)
    {
        Source = RawTextureVector[i];
        if (Destination.x + Source->w > MainTextureSize)   // TODO Improve arangment to waste less space
        {
            Destination.x = 0;
            Destination.y += LargestTextureSize;
        }

        Destination.w = Source->w;
        Destination.h = Source->h;
        SDL_BlitSurface(Source, &Source->clip_rect, AgragateSurface, &Destination);

        TextureCorners = Destination;
        TextureCorners.w = Destination.x + Destination.w;
        TextureCorners.h = Destination.y + Destination.h;

        TextureCordinates.push_back(TextureCorners);
        Destination.x += Source->w;

        SDL_FreeSurface(Source);
    }

    glGenTextures(1, &MainTexture);
    glBindTexture(GL_TEXTURE_2D, MainTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, 4, AgragateSurface->w, AgragateSurface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, AgragateSurface->pixels);

    if(true)  // Mip Mapping, dosn't seem to look good
    {
        glEnable(GL_TEXTURE_2D);

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 4, AgragateSurface->w, AgragateSurface->h, GL_RGBA, GL_UNSIGNED_BYTE, AgragateSurface->pixels);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }

    SDL_FreeSurface(AgragateSurface);
}

int TextureManager::round(double x)
{
	return (int)(x + 0.5);
}

int TextureManager::nextpoweroftwo(int x)
{
	double logbase2 = log(x) / log(2);
	return round(pow(2, ceil(logbase2)));
}

void TextureManager::BindTexturePoint(Uint32 TextureID, Uint8 Corner)
{
    switch(Corner % 4)
    {
        case 0:
        {
            glTexCoord2i(TextureCordinates[TextureID].x, TextureCordinates[TextureID].y);
            break;
        }
        case 1:
        {
            glTexCoord2i(TextureCordinates[TextureID].x, TextureCordinates[TextureID].h);
            break;
        }
        case 2:
        {
            glTexCoord2i(TextureCordinates[TextureID].w, TextureCordinates[TextureID].h);
            break;
        }
        case 3:
        {
            glTexCoord2i(TextureCordinates[TextureID].w, TextureCordinates[TextureID].y);
            break;
        }
    }
}
