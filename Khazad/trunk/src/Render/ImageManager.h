#ifndef IMAGE__HEADER
#define IMAGE__HEADER

#include <stdafx.h>

#include <Singleton.h>

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

class ImagePage;
class ClipImage;

class ImageManager
{
	DECLARE_SINGLETON_CLASS(ImageManager)

public:

	~ImageManager();
	bool Init();

	std::vector<ImagePage*> ImageLibrary;
	std::vector<ClipImage*> ClipLibrary;
	std::vector<ILuint> DevilImageVector;

	void loadClippedSurface(char* filepath, int cliphight, int clipwidth, int rows, int columns, bool ColorKey = false);
	ClipImage* loadSingleSurface(char* filepath, bool ColorKey = false);

    ILuint GenerateMaterialImage(Sint16 MaterialID, Sint16 SurfaceTypeID);

    ILuint GenerateGradientImage(ILuint TextureDevILID, Sint16 PrimaryColorID, Sint16 SecondaryColorID, Sint16 BorderColorID);
    ILuint GeneratedOverLayImage(ILuint TextureDevILID, Sint16 PrimaryColorID, Sint16 BorderColorID);
    ILuint GenerateKeeperImage(ILuint TextureDevILID, Sint16 BorderColorID);

    void ApplyBorder(ILuint DevilImageID, Sint32 BorderColorID);

    void ReportDevILErrors();
    SDL_Surface* loadSurface(char* filepath, bool ColorKey = false);

    ILuint loadImage(char* filepath, bool ColorKey = false);

protected:

};

#define IMAGE (ImageManager::GetInstance())

#endif // IMAGE_HEADER
