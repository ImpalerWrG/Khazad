//#ifndef IMAGE__HEADER
//#define IMAGE__HEADER

#include <stdafx.h>

#include <SDL_image.h>

class ImagePage;
class ClipImage;

class ImageManager
{
public:

	ImageManager();
	~ImageManager();

	std::vector<ImagePage*> ImageLibrary;
	std::vector<ClipImage*> ClipLibrary;

	void loadClippedSurface(char* filepath, int cliphight, int clipwidth, int rows, int columns, bool ColorKey = false, bool bmp = false);
	ClipImage* loadSingleSurface(char* filepath, bool ColorKey = false, bool bmp = false);

private:

	SDL_Surface* loadBMPSurface(char* filepath, bool ColorKey = false);
	SDL_Surface* loadSurface(char* filepath, bool ColorKey = false);
};

//#endif // IMAGE_HEADER
