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

	void loadClippedSurface(char* filepath, int cliphight, int clipwidth, int rows, int columns, bool ColorKey = false);
	ClipImage* loadSingleSurface(char* filepath, bool ColorKey = false);

    void ReportDevILErrors();
    SDL_Surface* loadSurface(char* filepath, bool ColorKey = false);

    ILuint loadImage(char* filepath, bool ColorKey = false);

protected:

};

#define IMAGE (ImageManager::GetInstance())

#endif // IMAGE_HEADER
