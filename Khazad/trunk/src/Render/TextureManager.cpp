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
    // set invalid texture
    currentTexture = 0;

    ilClearColour(0, 0, 0, 0);

    TextureDescriptor deadbeef;
    // Load the terrain Textures
    ILuint ILtex;
    glEnable(GL_TEXTURE_2D);
    for(Uint32 i = 0; i < DATA->getNumMaterials(); ++i)
    {
        ILtex = GenerateMaterialTexture(i);
        if(ILtex != 0)
        {
            DevilImageVector.push_back(ILtex);

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

/*
            char buffer[256];
            sprintf(buffer, "ScreenShots\\TexturePalete.png");
            ilSaveImage(Path(buffer));
*/


        }
    }

    // Merge to create Terrain Pallette
    MergeTextures();

	return true;
}

ILuint TextureManager::GenerateMaterialTexture(Uint16 MaterialID)
{
    //TODO: implement caching?
    //TODO: investigate possible memory leaks
    ILuint TextureID = IMAGE->loadImage(DATA->getTextureData(DATA->getMaterialData(MaterialID)->getTextureID())->getPath(), false);
    // the source image is bound now, its format is BGRA
    string colormode = DATA->getMaterialData(MaterialID)->getColorMode();
    if(colormode.empty())
        colormode = "gradientmap";

    if(colormode == "gradientmap")
    {
        ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_BYTE);
        Uint8* TextureData = ilGetData();
        Uint32 width = ilGetInteger(IL_IMAGE_WIDTH);
        Uint32 height = ilGetInteger(IL_IMAGE_HEIGHT);

        ILuint MaskID;
        ilGenImages(1, &MaskID);
        ilBindImage(MaskID);
        ilTexImage(width, height, 1, 4, IL_BGRA, IL_UNSIGNED_BYTE, NULL);
        uint8_t* MaskData = ilGetData();

        ColorData* PrimaryColor = DATA->getColorData(DATA->getMaterialData(MaterialID)->getPrimaryColorID());
        ColorData* SecondaryColor = DATA->getColorData(DATA->getMaterialData(MaterialID)->getSecondaryColorID());

        Uint32 bpp = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
        for(Uint32 i = 0; i < width; i++)
        {
            for(Uint32 j = 0; j < height; j++)
            {
                MaskData[(i * width * bpp) + (j * bpp) + 0] = SecondaryColor->getBlue(); // Blue
                MaskData[(i * width * bpp) + (j * bpp) + 1] = SecondaryColor->getGreen(); // Green
                MaskData[(i * width * bpp) + (j * bpp) + 2] = SecondaryColor->getRed(); // Red
                MaskData[(i * width * bpp) + (j * bpp) + 3] = 255 - TextureData[(i * width) + j]; // Alpha
            }
        }

        ILuint BaseID;
        ilGenImages(1, &BaseID);
        ilBindImage(BaseID);
        ilTexImage(width, height, 1, 4, IL_BGRA, IL_UNSIGNED_BYTE, NULL);
        uint8_t* BaseData = ilGetData();

        for(Uint32 i = 0; i < width; i++)
        {
            for(Uint32 j = 0; j < height; j++)
            {
                BaseData[(i * width * bpp) + (j * bpp) + 0] = PrimaryColor->getBlue(); // Blue
                BaseData[(i * width * bpp) + (j * bpp) + 1] = PrimaryColor->getGreen(); // Green
                BaseData[(i * width * bpp) + (j * bpp) + 2] = PrimaryColor->getRed(); // Red
                BaseData[(i * width * bpp) + (j * bpp) + 3] = 255; // Alpha
            }
        }

        ilOverlayImage(MaskID, 0, 0, 0);

        if (DATA->getMaterialData(MaterialID)->getBorderColorID() != -1)
        {
            ApplyBorder(BaseData, width, height, bpp, DATA->getMaterialData(MaterialID)->getBorderColorID());
        }

        iluFlipImage();
        return BaseID;
    }
    else if(colormode == "overlay")
    {
        ilConvertImage(IL_LUMINANCE_ALPHA, IL_UNSIGNED_BYTE);
        Uint8* TextureData = ilGetData();
        Uint32 width = ilGetInteger(IL_IMAGE_WIDTH);
        Uint32 height = ilGetInteger(IL_IMAGE_HEIGHT);

        ColorData* PrimaryColor = DATA->getColorData(DATA->getMaterialData(MaterialID)->getPrimaryColorID());
        //ColorData SecondaryColor = DATA->getMaterialData(MaterialID)->getSecondaryColorID();

        ILuint BaseID;
        ilGenImages(1, &BaseID);
        ilBindImage(BaseID);
        ilTexImage(width, height, 1, 4, IL_BGRA, IL_UNSIGNED_BYTE, NULL);
        uint8_t* BaseData = ilGetData();

        Uint32 bpp = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);

        for(Uint32 i = 0; i < width; i++)
        {
            for(Uint32 j = 0; j < height; j++)
            {
                float Base  = TextureData[(i * width * 2) + (j * 2) + 0];
                uint8_t Alpha =  TextureData[(i * width * 2) + (j * 2) + 1];
                // HACK: fix issues with transparency weirdness
                if(Alpha == 0) Alpha = 1;
                float fAlpha = Alpha;
                Base /= 255.0;
                float OB = PrimaryColor->getBlue();
                OB /= 255.0;

                float OG = PrimaryColor->getGreen();
                OG /= 255.0;

                float OR = PrimaryColor->getRed();
                OR /= 255.0;

                // coloring using overlay mode
                if(Base >= 0.5)
                {
                    BaseData[(i * width * bpp) + (j * bpp) + 0] = (1.0 - 2.0 * (1.0 - OB) * (1.0 - Base)) * 255;
                    BaseData[(i * width * bpp) + (j * bpp) + 1] = (1.0 - 2.0 * (1.0 - OG) * (1.0 - Base)) * 255;
                    BaseData[(i * width * bpp) + (j * bpp) + 2] = (1.0 - 2.0 * (1.0 - OR) * (1.0 - Base)) * 255;
                    BaseData[(i * width * bpp) + (j * bpp) + 3] = fAlpha; // Alpha
                }
                else
                {
                    BaseData[(i * width * bpp) + (j * bpp) + 0] = (2.0* OB * Base) * 255; // Blue
                    BaseData[(i * width * bpp) + (j * bpp) + 1] = (2.0* OG * Base) * 255; // Green
                    BaseData[(i * width * bpp) + (j * bpp) + 2] = (2.0* OR * Base) * 255; // Red
                    BaseData[(i * width * bpp) + (j * bpp) + 3] = fAlpha; // Alpha
                }
            }
        }

        if (DATA->getMaterialData(MaterialID)->getBorderColorID() != -1)
        {
            ApplyBorder(BaseData, width, height, bpp, DATA->getMaterialData(MaterialID)->getBorderColorID());
        }

        iluFlipImage();
        return BaseID;
    }
    else if(colormode == "keepimage")
    {
        if (DATA->getMaterialData(MaterialID)->getBorderColorID() != -1)
        {
            Uint32 bpp = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
            Uint32 width = ilGetInteger(IL_IMAGE_WIDTH);
            Uint32 height = ilGetInteger(IL_IMAGE_HEIGHT);
            Uint8* TextureData = ilGetData();

            ApplyBorder(TextureData, width, height, bpp, DATA->getMaterialData(MaterialID)->getBorderColorID());
        }
        return TextureID;
    }
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
// bind separate texture
void TextureManager::BindTexture(Uint32 TextureID)
{
    GLuint tex = TextureCache[TextureID].GLtexture;
    if( tex == currentTexture ) return;
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
