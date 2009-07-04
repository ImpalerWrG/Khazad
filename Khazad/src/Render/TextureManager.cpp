#include <stdafx.h>

#include <ImageManager.h>
#include <ClipImage.h>
#include <TextureManager.h>
#include <DataManager.h>


DECLARE_SINGLETON(TextureManager)

bool TextureManager::Init()
{
    // Version Check of DevIL
    if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION || iluGetInteger(ILU_VERSION_NUM) < ILU_VERSION || ilutGetInteger(ILUT_VERSION_NUM) < ILUT_VERSION)
    {
		printf ("DevIL library is out of date! Please upgrade\n");
		return false;
	}

    // Initilize Devil with OpenGL rendering support
    ilInit();
    iluInit();
    ilutInit();
    ilutRenderer(ILUT_OPENGL);
    ilutEnable(ILUT_OPENGL_CONV);

    ilEnable (IL_CONV_PAL);

    //ilEnable(IL_ORIGIN_SET);
    //ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

    // Load the terrain Textures
    for(Uint32 i = 0; i < DATA->getNumTextures(); ++i)
    {
        ILuint TextureImage = IMAGE->loadImage(DATA->getTextureData(i)->getPath(), false);

        if(TextureImage != 0)
        {
            DevilImageVector.push_back(TextureImage);
        }
    }

    // Merge to create Terrain Pallette
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

ILuint TextureManager::loadTextureSingular(char* filepath, bool ColorKey)
{
    ILuint ImageID;
    ilGenImages(1, &ImageID);
    ilBindImage(ImageID);

    printf("Loading Image file: %s\n", filepath);
    ilLoadImage(filepath);

    ReportDevILErrors();
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
