#include <stdafx.h>

#include <ImageManager.h>
#include <TextureManager.h>
#include <DataManager.h>


DECLARE_SINGLETON(TextureManager)

TextureManager::TextureManager()
{

}

TextureManager::~TextureManager()
{

}

bool TextureManager::Init()
{
    glEnable(GL_TEXTURE_2D);

    for(Uint32 i = 0; i < DATA->getNumMaterials(); ++i)
    {
        ILuint DevILImageID = IMAGE->GenerateMaterialImage(i);
        if(DevILImageID != 0)
        {
            TextureCache.push_back(GenerateTexture(DevILImageID));
        }
    }
    // FIXME: MergeTextures causes GL errors
    //MergeTextures();
	return true;
}

GLuint TextureManager::GenerateTexture(ILuint DevILImageID)
{
    printf ("Generating Texture: %i\n", (int) DevILImageID);

    ilBindImage(DevILImageID);

    GLuint GLtextureID;
    glGenTextures(1, &GLtextureID);
    glBindTexture(GL_TEXTURE_2D, GLtextureID);

    gluBuild2DMipmaps(GL_TEXTURE_2D, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    // the texture wraps over at the edges (repeat)
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST );

    // when texture area is large, bilinear filter the original
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    return GLtextureID;
}

bool TextureManager::isFileEnding(const char* FilePath, const char* Ending)
{
    Uint16 PathLength = strlen(FilePath);
    Uint16 EndingLength = strlen(Ending);
    Uint16 EndingStart = PathLength - EndingLength;

    return strcmp(FilePath + EndingStart,  Ending) == 0;
}

void TextureManager::MergeTextures()
{
    printf("Creating Texture Palette from %i individual textures\n", IMAGE->DevilImageVector.size());

    Uint32 LargestTextureSize = 64;  // TODO Must be found dynamicly, see bellow
    float root = sqrt((float) IMAGE->DevilImageVector.size());
    Uint16 MainTextureSize = nextpoweroftwo(round(root)) * LargestTextureSize;

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

    for(Uint32 i = 0; i < IMAGE->DevilImageVector.size(); i++)
    {
        Sint16 Width = LargestTextureSize; //get this from image for varable size textures;
        Sint16 Hight = LargestTextureSize; //get this from image for varable size textures;

        if (DestinationX + Width > MainTextureSize)
        {
            DestinationX = 0;
            DestinationY += Hight;
        }
        ilOverlayImage(IMAGE->DevilImageVector[i], (ILuint) DestinationX, (ILuint) DestinationY, 0);
        ReportDevILErrors();

        TextureCorners.x = DestinationX;
        TextureCorners.y = DestinationY;
        TextureCorners.w = DestinationX + Width;
        TextureCorners.h = DestinationY + Hight;

        DestinationX += Width;
    }

    iluFlipImage();
    ReportDevILErrors();

    GLuint MainTexture;
    glGenTextures(1, &MainTexture);
    glBindTexture(GL_TEXTURE_2D, MainTexture);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    printf("Binding Texture Palette to OpenGL\n");
    MainTexture = ilutGLBindTexImage();
    ReportDevILErrors();

    //glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());

    char buffer[256];
    sprintf(buffer, "ScreenShots\\TexturePalete.png");
    ilSaveImage(Path(buffer));

    if(true) // Mip Mapping, dosn't seem to look good but better then without
    {
        glEnable(GL_TEXTURE_2D);

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
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

void TextureManager::ResetTextureBinding()
{
    currentTexture = 0;
}

void TextureManager::BindTexture(Uint32 TextureID)
{
    GLuint tex = TextureCache[TextureID];
    if( tex != currentTexture )
    {
        currentTexture = tex;
        glBindTexture(GL_TEXTURE_2D, tex);
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
