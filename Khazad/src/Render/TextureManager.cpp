#include <stdafx.h>

#include <ImagePage.h>
#include <ClipImage.h>
#include <TextureManager.h>


DECLARE_SINGLETON(TextureManager)

bool TextureManager::Init()
{

// 0 Grass
	loadTextureSingular("Assets\\Textures\\grass.png", false, false);
// 1 Stone
	loadTextureSingular("Assets\\Textures\\stone.png", false, false);
// 2 Murky Pool
	loadTextureSingular("Assets\\Textures\\Pool.png", false, false);
// 3 Slope ?  Fire
	loadTextureSingular("Assets\\Textures\\lava.png", false, false);
// 4 Constructed Stairs & Ramps
	loadTextureSingular("Assets\\Textures\\Brick.png", false, false);
// 5 OpenSpace(Air)
	loadTextureSingular("Assets\\Textures\\Air.png", false, false);
// 6 Not Assigned
	loadTextureSingular("Assets\\Textures\\nehe.png", false, false);
// 7 Detailed Stone / Constructions
	loadTextureSingular("Assets\\Textures\\floor_detailed.png", false, false);
// 8 Driftwood
	loadTextureSingular("Assets\\Textures\\Bark.png", false, false);
// 9 Misilanious Stone
	loadTextureSingular("Assets\\Textures\\StoneWall.png.png", false, false);
// 10 Soil ?
	loadTextureSingular("Assets\\Textures\\soil.png", false, false);
// 11 Soil
	loadTextureSingular("Assets\\Textures\\Sand.png", false, false);
// 12 Pebbles
	loadTextureSingular("Assets\\Textures\\Cobblestone.png", false, false);
// 13 Dead Shrub/Tree/Sappling
	loadTextureSingular("Assets\\Textures\\grass_dry.png", false, false);
// 14 Shrub
	loadTextureSingular("Assets\\Textures\\Shrub.png", false, false);
// 15 Sappling/Tree
	loadTextureSingular("Assets\\Textures\\Bark.png", false, false);
// 16 Rock
	loadTextureSingular("Assets\\Textures\\stone.png", false, false);
// 17 ??
	loadTextureSingular("Assets\\Textures\\Boards.png", false, false);
// 18 RawStone
	loadTextureSingular("Assets\\Textures\\RawStone.png", false, false);
// 19 all Boulders
	loadTextureSingular("Assets\\Textures\\boulder.png", false, false);
// 20
	loadTextureSingular("Assets\\Textures\\water.png", false, false);
// 21 Furrowed Soil
	loadTextureSingular("Assets\\Textures\\Dirt.png", false, false);
// 22 Stone Constructions / Veins, Gems, Clusters
	loadTextureSingular("Assets\\Textures\\metal.png", false, false);
// 23
	loadTextureSingular("Assets\\Textures\\nehe.png", false, false);
// 24 Lava
	loadTextureSingular("Assets\\Textures\\lava.png", false, false);
// 25 Ice-1
	loadTextureSingular("Assets\\Textures\\Ice.png", false, false);
// 26
	loadTextureSingular("Assets\\Textures\\nehe.png", false, false);
// 27 Ice-2
	loadTextureSingular("Assets\\Textures\\snow.png", false, false);
// 28
	loadTextureSingular("Assets\\Textures\\nehe.png", false, false);
// 29
	loadTextureSingular("Assets\\Textures\\nehe.png", false, false);
// 30
	loadTextureSingular("Assets\\Textures\\constructed_wall.png", false, false);
// 31 Chasm/Pit
	loadTextureSingular("Assets\\Textures\\Pit.png", false, false);
// 32 Obsidian/Ash
	loadTextureSingular("Assets\\Textures\\obsidian.png", false, false);
// 33 dead grass
	loadTextureSingular("Assets\\Textures\\grass_dry.png", false, false);


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


    if (TextureImage)
	{
        RawTextureVector.push_back(TextureImage);
	}
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
    }

    glGenTextures(1, &MainTexture);
    glBindTexture(GL_TEXTURE_2D, MainTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, 4, AgragateSurface->w, AgragateSurface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, AgragateSurface->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

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


