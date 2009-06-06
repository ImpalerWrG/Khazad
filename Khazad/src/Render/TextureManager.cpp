#include <stdafx.h>

#include <ImagePage.h>
#include <ClipImage.h>
#include <TextureManager.h>
#include <DataManager.h>


DECLARE_SINGLETON(TextureManager)

bool TextureManager::Init()
{
    // Initilize Devil with OpenGL rendering support
    ilInit();
    iluInit();
    ilutRenderer(ILUT_OPENGL);

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

bool TextureManager::isFileEnding(const char* FilePath, const char* Ending)
{
    Uint16 PathLength = strlen(FilePath);
    Uint16 EndingLength = strlen(Ending);
    Uint16 EndingStart = PathLength - EndingLength;

    return strcmp(FilePath + EndingStart,  Ending) == 0;
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

ILuint TextureManager::loadTextureSingular(char* filepath, bool ColorKey, bool bmp)
{
	unsigned char* RawImage = NULL;
    int width, height, channels;

    ILuint ImageID;
    ilGenImages(1, &ImageID);
    ilBindImage(ImageID);

    printf("Loading Image file: %s\n", filepath);
    ilLoadImage(filepath);

    ILenum Error;
    while ((Error = ilGetError()) != IL_NO_ERROR)
    {
        printf("DevIL Error %d: %s\n", Error, iluErrorString(Error));
    }
    //ilConvertImage(IL_BGRA, IL_UNSIGNED_BYTE);
    // Perhapse re-size image?

    DevilImageVector.push_back(ImageID);
	return ImageID;
}

void TextureManager::MergeTextures()
{
    printf("Creating Texture Palette from %i individual textures\n", DevilImageVector.size());

    Uint32 LargestTextureSize = 64;  // TODO Must be found dynamicly, see bellow
    float root = sqrt((float) DevilImageVector.size());
    MainTextureSize = nextpoweroftwo(round(root)) * LargestTextureSize;

    ReportDevILErrors();

    // Devil
    ILuint AgragateImageID;
    ilGenImages(1, &AgragateImageID);
    ilBindImage(AgragateImageID);

    // Create a blank page to fill with textures
    ilTexImage(MainTextureSize, MainTextureSize, 1, 4, IL_BGRA, IL_UNSIGNED_BYTE, NULL);

    ReportDevILErrors();

    Sint16 DestinationX = 0;
    Sint16 DestinationY = 0;

    ilDisable(IL_BLIT_BLEND);
    SDL_Rect TextureCorners;

    for(Uint32 i = 0; i < DevilImageVector.size(); i++)
    {
        Sint16 Width = LargestTextureSize; //get this from image for varable size textures;
        Sint16 Hight = LargestTextureSize; //get this from image for varable size textures;

        if (DestinationX + Width > MainTextureSize)
        {
            DestinationX = 0;
            DestinationY += Hight;
        }
        ilOverlayImage(DevilImageVector[i], (ILuint) DestinationX, (ILuint) DestinationY, 0);
        ReportDevILErrors();

        TextureCorners.x = DestinationX;
        TextureCorners.y = DestinationY;
        TextureCorners.w = DestinationX + Width;
        TextureCorners.h = DestinationY + Hight;

        TextureCordinates.push_back(TextureCorners);

        DestinationX += Width;
    }

    iluFlipImage();
    ilutEnable(ILUT_OPENGL_CONV);
    ReportDevILErrors();

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &MainTexture);
    glBindTexture(GL_TEXTURE_2D, MainTexture);

    printf("Binding Texture Palette to OpenGL\n");
    MainTexture = ilutGLBindTexImage();
    ReportDevILErrors();

    //glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());


    if(true) // Mip Mapping, dosn't seem to look good but better then without
    {
        glEnable(GL_TEXTURE_2D);

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        gluBuild2DMipmaps(GL_TEXTURE_2D, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
    }
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

void TextureManager::ReportDevILErrors()
{
    ILenum Error;
    while ((Error = ilGetError()) != IL_NO_ERROR)
    {
        printf("DevIL Error %d: %s\n", Error, iluErrorString(Error));
    }
}
