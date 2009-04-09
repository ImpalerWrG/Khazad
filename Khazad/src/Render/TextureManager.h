#ifndef TEXTURE__HEADER
#define TEXTURE__HEADER

#include <stdafx.h>

#include <Singleton.h>


class ImagePage;
class ClipImage;

class TextureManager
{
	DECLARE_SINGLETON_CLASS(TextureManager)

public:

	GLuint Tex[1];

	~TextureManager();
	bool Init();

	std::vector<ImagePage*> ImageLibrary;
	std::vector<ClipImage*> ClipLibrary;
	std::vector<GLuint> SingularTextureLibrary;
	std::vector<GLuint**> PagedTextureLibrary;

	void loadClippedSurface(char* filepath, int cliphight, int clipwidth, int rows, int columns, bool ColorKey = false, bool bmp = false);
	ClipImage* loadSingleSurface(char* filepath, bool ColorKey = false, bool bmp = false);

	SDL_Surface* loadBMPSurface(char* filepath, bool ColorKey = false);
	SDL_Surface* loadSurface(char* filepath, bool ColorKey = false);

	void loadTextureSingular(char* filepath, bool ColorKey = false, bool bmp = false);
	void loadTexturePaged(char* filepath, int cliphight, int clipwidth, int rows, int columns, bool ColorKey = false, bool bmp = false);


private:



};

#define TEXTURE (TextureManager::GetInstance())

#endif // TEXTURE__HEADER
