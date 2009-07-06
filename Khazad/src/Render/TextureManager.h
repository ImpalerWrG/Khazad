#ifndef TEXTURE__HEADER
#define TEXTURE__HEADER

#include <stdafx.h>

#include <Singleton.h>

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>


class ImagePage;
class ClipImage;

class TextureManager
{
	DECLARE_SINGLETON_CLASS(TextureManager)

public:

	~TextureManager();
	bool Init();

    void ApplyBorder(Uint8* ImageData, Uint32 width, Uint32 height, Uint32 bpp, Uint8 Red, Uint8 Green, Uint8 Blue);
    void MergeTextures();

    int round(double x);
    int nextpoweroftwo(int x);

    ILuint GenerateMaterialTexture(Uint16 MaterialID);

    GLuint getAggragateTexture()        { return MainTexture; }
    Uint16 getTextureCount()            { return TextureCordinates.size(); }
    int getAggragateTextureSize()       { return MainTextureSize; }

    void BindTexturePoint(Uint32 TextureID, Uint8 Corner = 0);

    void ReportDevILErrors();

protected:

	std::vector<ImagePage*> ImageLibrary;
	std::vector<ClipImage*> ClipLibrary;
	std::vector<GLuint> SingularTextureLibrary;
	std::vector<GLuint**> PagedTextureLibrary;

	std::vector<ILuint> DevilImageVector;
	std::vector<SDL_Rect> TextureCordinates;

    bool isFileEnding(const char* FilePath, const char* Ending);

    SDL_Surface* AgragateSurface;
	GLuint MainTexture;
	int MainTextureSize;

};

#define TEXTURE (TextureManager::GetInstance())

#endif // TEXTURE__HEADER
