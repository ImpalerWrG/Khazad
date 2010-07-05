#ifndef IMAGE__HEADER
#define IMAGE__HEADER

#include <stdafx.h>

#include <Singleton.h>

#include <IL/il.h>
#include <IL/ilu.h>
//#include <IL/ilut.h>

class ImageManager
{
	DECLARE_SINGLETON_CLASS(ImageManager)

public:

	~ImageManager();
	bool Init();

    ILuint GenerateMaterialImage(int16_t MaterialID, int16_t TextureID);

    ILuint ClipImage(ILuint SourceID, ILuint X, ILuint Y, ILuint W, ILuint H);

    ILuint GenerateGradientImage(ILuint TextureDevILID, int16_t PrimaryColorID, int16_t SecondaryColorID, int16_t BorderColorID);
    ILuint GeneratedOverLayImage(ILuint TextureDevILID, int16_t PrimaryColorID, int16_t BorderColorID);
    ILuint GenerateKeeperImage(ILuint TextureDevILID, int16_t BorderColorID);

    void ApplyBorder(ILuint DevilImageID, int32_t BorderColorID);

    uint8_t* getImageData(ILuint DevilImageID);
    uint16_t getImageWidth(ILuint DevilImageID);
    uint16_t getImageHeight(ILuint DevilImageID);

    void ReportDevILErrors();

    ILuint loadImage(char* filepath, bool ColorKey = false);

protected:

};

#define IMAGE (ImageManager::GetInstance())

#endif // IMAGE_HEADER
