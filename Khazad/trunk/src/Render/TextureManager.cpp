#include <stdafx.h>

#include <ImageManager.h>
#include <ClipImage.h>
#include <TextureManager.h>
#include <DataManager.h>


DECLARE_SINGLETON(TextureManager)

bool TextureManager::Init()
{
    TextureDescriptor deadbeef;
    // Load the terrain Textures

    ILuint ILtex;
    glEnable(GL_TEXTURE_2D);
    for(Uint32 i = 0; i < DATA->getNumMaterials(); ++i)
    {
        ILtex = GenerateMaterialTexture(i);
        if(ILtex != 0)
        {
            //DevilImageVector.push_back(ILtex);

            GLuint GLtex;
            glGenTextures(1, &GLtex);
            printf ("GLGENTEXTURES: %d\n");
            glBindTexture(GL_TEXTURE_2D, GLtex);

            gluBuild2DMipmaps(GL_TEXTURE_2D, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
            glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

            // the texture wraps over at the edges (repeat)
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST );

            // when texture area is large, bilinear filter the original
            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
            deadbeef.used = 1;
            deadbeef.GLtexture = GLtex;
            deadbeef.ILtexture = ILtex;

            TextureCache.push_back(deadbeef);


            char buffer[256];
            sprintf(buffer, "ScreenShots\\TexturePalete.png");
            ilSaveImage(Path(buffer));

        }
    }

    // Merge to create Terrain Pallette
    MergeTextures();

	return true;
}

ILuint TextureManager::GenerateMaterialTexture(Uint16 MaterialID)
{
    ILuint TextureDevILID = DATA->getTextureData(DATA->getMaterialData(MaterialID)->getTextureID())->getDevILID();

    Sint16 PrimaryColorID = DATA->getMaterialData(MaterialID)->getPrimaryColorID();
    Sint16 SecondaryColorID = DATA->getMaterialData(MaterialID)->getSecondaryColorID();
    Sint16 BorderColorID = DATA->getMaterialData(MaterialID)->getBorderColorID();

    // the source image is bound now, its format is BGRA
    string colormode = DATA->getMaterialData(MaterialID)->getColorMode();

    if(colormode.empty() || colormode == "gradientmap")
    {
        return GenerateGradientTexture(TextureDevILID, PrimaryColorID, SecondaryColorID, BorderColorID);
    }
    else if(colormode == "overlay")
    {
        return GeneratedOverLayTexture(TextureDevILID, PrimaryColorID, BorderColorID);
    }
    else if(colormode == "keepimage")
    {
        return GenerateKeeperTexture(TextureDevILID, BorderColorID);
    }
}

ILuint TextureManager::GenerateGradientTexture(ILuint TextureDevILID, Sint16 PrimaryColorID, Sint16 SecondaryColorID, Sint16 BorderColorID)
{
    ILuint TextureImageID;
    ilGenImages(1, &TextureImageID);
    ilBindImage(TextureImageID);
    ilCopyImage(TextureDevILID);
    ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_BYTE);  //Load as IL_LUMINANCE to avoid convertion?

    Uint8* TextureImageData = ilGetData();
    Uint32 width = ilGetInteger(IL_IMAGE_WIDTH);
    Uint32 height = ilGetInteger(IL_IMAGE_HEIGHT);

    ILuint MaskImageID;
    ilGenImages(1, &MaskImageID);
    ilBindImage(MaskImageID);
    ilTexImage(width, height, 1, 4, IL_BGRA, IL_UNSIGNED_BYTE, NULL);
    Uint8* MaskImageData = ilGetData();

    ColorData* PrimaryColor = DATA->getColorData(PrimaryColorID);
    ColorData* SecondaryColor = DATA->getColorData(SecondaryColorID);

    Uint32 bpp = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
    if(SecondaryColor != NULL)
    {
        for(Uint32 i = 0; i < width; i++)
        {
            for(Uint32 j = 0; j < height; j++)
            {
                MaskImageData[(i * width * bpp) + (j * bpp) + 0] = SecondaryColor->getBlue();     // Blue
                MaskImageData[(i * width * bpp) + (j * bpp) + 1] = SecondaryColor->getGreen();    // Green
                MaskImageData[(i * width * bpp) + (j * bpp) + 2] = SecondaryColor->getRed();      // Red
                MaskImageData[(i * width * bpp) + (j * bpp) + 3] = 255 - TextureImageData[(i * width) + j]; // Alpha
            }
        }
    }

    ILuint NewImageID;
    ilGenImages(1, &NewImageID);
    ilBindImage(NewImageID);
    ilTexImage(width, height, 1, 4, IL_BGRA, IL_UNSIGNED_BYTE, NULL);
    Uint8* NewImageData = ilGetData();

    if(PrimaryColor != NULL)
    {
        for(Uint32 i = 0; i < width; i++)
        {
            for(Uint32 j = 0; j < height; j++)
            {
                NewImageData[(i * width * bpp) + (j * bpp) + 0] = PrimaryColor->getBlue(); // Blue
                NewImageData[(i * width * bpp) + (j * bpp) + 1] = PrimaryColor->getGreen(); // Green
                NewImageData[(i * width * bpp) + (j * bpp) + 2] = PrimaryColor->getRed(); // Red
                NewImageData[(i * width * bpp) + (j * bpp) + 3] = 255; // Alpha
            }
        }
    }

    ilOverlayImage(MaskImageID, 0, 0, 0);

    if (BorderColorID != -1)
    {
        ApplyBorder(NewImageData, width, height, bpp, BorderColorID);
    }

    iluFlipImage();
    return NewImageID;
}

ILuint TextureManager::GeneratedOverLayTexture(ILuint TextureDevILID, Sint16 PrimaryColorID, Sint16 BorderColorID)
{
    ILuint TextureImageID;
    ilGenImages(1, &TextureImageID);
    ilBindImage(TextureImageID);
    ilCopyImage(TextureDevILID);
    ilConvertImage(IL_LUMINANCE_ALPHA, IL_UNSIGNED_BYTE);

    Uint8* TextureImageData = ilGetData();
    Uint32 width = ilGetInteger(IL_IMAGE_WIDTH);
    Uint32 height = ilGetInteger(IL_IMAGE_HEIGHT);

    ColorData* PrimaryColor = DATA->getColorData(PrimaryColorID);

    ILuint NewImageID;
    ilGenImages(1, &NewImageID);
    ilBindImage(NewImageID);
    ilTexImage(width, height, 1, 4, IL_BGRA, IL_UNSIGNED_BYTE, NULL);
    Uint8* NewImageData = ilGetData();

    Uint32 bpp = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);

    if(PrimaryColor != NULL)
    {
        for(Uint32 i = 0; i < width; i++)
        {
            for(Uint32 j = 0; j < height; j++)
            {
                float Base  = TextureImageData[(i * width * 2) + (j * 2) + 0];
                Uint8 Alpha =  TextureImageData[(i * width * 2) + (j * 2) + 1];
                Base /= 255.0;

                float OriginalBlue = PrimaryColor->getBlue();
                OriginalBlue /= 255.0;

                float OriginalGreen = PrimaryColor->getGreen();
                OriginalGreen /= 255.0;

                float OriginalRed = PrimaryColor->getRed();
                OriginalRed /= 255.0;

                // coloring using overlay mode
                if(Base >= 0.5)
                {
                    NewImageData[(i * width * bpp) + (j * bpp) + 0] = (1.0 - 2.0 * (1.0 - OriginalBlue) * (1.0 - Base)) * 255; // Blue
                    NewImageData[(i * width * bpp) + (j * bpp) + 1] = (1.0 - 2.0 * (1.0 - OriginalGreen) * (1.0 - Base)) * 255; // Green
                    NewImageData[(i * width * bpp) + (j * bpp) + 2] = (1.0 - 2.0 * (1.0 - OriginalRed) * (1.0 - Base)) * 255; // Red
                    NewImageData[(i * width * bpp) + (j * bpp) + 3] = Alpha;
                }
                else
                {
                    NewImageData[(i * width * bpp) + (j * bpp) + 0] = (2.0 * OriginalBlue * Base) * 255; // Blue
                    NewImageData[(i * width * bpp) + (j * bpp) + 1] = (2.0 * OriginalGreen * Base) * 255; // Green
                    NewImageData[(i * width * bpp) + (j * bpp) + 2] = (2.0 * OriginalRed * Base) * 255; // Red
                    NewImageData[(i * width * bpp) + (j * bpp) + 3] = Alpha;
                }
            }
        }
    }

    if (BorderColorID != -1)
    {
        ApplyBorder(NewImageData, width, height, bpp, BorderColorID);
    }

    iluFlipImage();
    return NewImageID;
}

ILuint TextureManager::GenerateKeeperTexture(ILuint TextureDevILID, Sint16 BorderColorID)
{
    ILuint NewImageID;
    ilGenImages(1, &NewImageID);
    ilBindImage(NewImageID);

    ilCopyImage(TextureDevILID);
    Uint32 width = ilGetInteger(IL_IMAGE_WIDTH);
    Uint32 height = ilGetInteger(IL_IMAGE_HEIGHT);

    ilTexImage(width, height, 1, 4, IL_BGRA, IL_UNSIGNED_BYTE, NULL);
    Uint8* NewImageData = ilGetData();

    if (BorderColorID != -1)
    {
        Uint32 bpp = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
        Uint32 width = ilGetInteger(IL_IMAGE_WIDTH);
        Uint32 height = ilGetInteger(IL_IMAGE_HEIGHT);
        Uint8* TextureData = ilGetData();

        ApplyBorder(NewImageData, width, height, bpp, BorderColorID);
    }
    return NewImageID;
}

void TextureManager::ApplyBorder(Uint8* ImageData, Uint32 width, Uint32 height, Uint32 bpp, Sint32 BorderColorID)
{
    Uint8 Red, Green, Blue;
    ColorData* BorderColor = DATA->getColorData(BorderColorID);

    if(BorderColor != NULL)
    {
        Red = BorderColor->getRed();
        Green = BorderColor->getGreen();
        Blue = BorderColor->getBlue();

        if(ImageData != NULL)
        {
            for(Uint32 i = 0; i < width; i++)
            {
                ImageData[(i * width * bpp) +  0] = Red;    // Red
                ImageData[(i * width * bpp) +  1] = Green;  // Green
                ImageData[(i * width * bpp) +  2] = Blue;   // Blue
                ImageData[(i * width * bpp) +  3] = 255;    // Alpha

                ImageData[(i * width * bpp) + ((height - 1) * bpp) + 0] = Red;      // Red
                ImageData[(i * width * bpp) + ((height - 1) * bpp) + 1] = Green;    // Green
                ImageData[(i * width * bpp) + ((height - 1) * bpp) + 2] = Blue;     // Blue
                ImageData[(i * width * bpp) + ((height - 1) * bpp) + 3] = 255;      // Alpha
            }

            for(Uint32 j = 0; j < height; j++)
            {
                ImageData[((width - 1) * height * bpp) + (j * bpp) + 0] = Red;      // Red
                ImageData[((width - 1) * height * bpp) + (j * bpp) + 1] = Green;    // Green
                ImageData[((width - 1) * height * bpp) + (j * bpp) + 2] = Blue;     // Blue
                ImageData[((width - 1) * height * bpp) + (j * bpp) + 3] = 255;      // Alpha

                ImageData[(j * bpp) + 0] = Red;     // Red
                ImageData[(j * bpp) + 1] = Green;   // Green
                ImageData[(j * bpp) + 2] = Blue;    // Blue
                ImageData[(j * bpp) + 3] = 255;     // Alpha
            }
        }
    }
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

void TextureManager::MergeTextures()
{
    printf("Creating Texture Palette from %i individual textures\n", IMAGE->DevilImageVector.size());

    Uint32 LargestTextureSize = 64;  // TODO Must be found dynamicly, see bellow
    float root = sqrt((float) IMAGE->DevilImageVector.size());
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

        TextureCordinates.push_back(TextureCorners);

        DestinationX += Width;
    }

    iluFlipImage();
    ReportDevILErrors();


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

void TextureManager::BindTexturePoint(Uint32 TextureID, float u, float v)
{
    glTexCoord2f(u,v);
}
// bind the texture aggregate
void TextureManager::BindAggregate( void )
{
    glBindTexture(GL_TEXTURE_2D, MainTexture);
}

void TextureManager::ResetTextureBinding()
{
    currentTexture = 0;
}


// bind separate texture
void TextureManager::BindTexture(Uint32 TextureID)
{
    GLuint tex = TextureCache[TextureID].GLtexture;
    if( tex == currentTexture )
        return;

    currentTexture = tex;
    glBindTexture(GL_TEXTURE_2D, TextureCache[TextureID].GLtexture);
}

void TextureManager::ReportDevILErrors()
{
    ILenum Error;
    while ((Error = ilGetError()) != IL_NO_ERROR)
    {
        printf("DevIL Error %d: %s\n", Error, iluErrorString(Error));
    }
}
