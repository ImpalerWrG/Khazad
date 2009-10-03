#ifndef TEXTURE__HEADER
#define TEXTURE__HEADER

#include <stdafx.h>

#include <Singleton.h>

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>


class ImagePage;
class ClipImage;

struct TextureDescriptor
{
    bool used;
    GLuint GLtexture;
    ILuint ILtexture;
};

class TextureManager
{
	DECLARE_SINGLETON_CLASS(TextureManager)

public:

	~TextureManager();
	bool Init();

    void ApplyBorder(Uint8* ImageData, Uint32 width, Uint32 height, Uint32 bpp, Sint32 ColorID);
    void MergeTextures();

    int round(double x);
    int nextpoweroftwo(int x);

    ILuint GenerateMaterialTexture(Uint16 MaterialID);

    //GLuint getAggragateTexture()        { return MainTexture; }
    //Uint16 getTextureCount()            { return TextureCordinates.size(); }
    //int getAggragateTextureSize()       { return MainTextureSize; }

    ILuint GenerateGradientTexture(ILuint TextureDevILID, Sint16 PrimaryColorID, Sint16 SecondaryColorID, Sint16 BorderColorID);
    ILuint GeneratedOverLayTexture(ILuint TextureDevILID, Sint16 PrimaryColorID, Sint16 BorderColorID);
    ILuint GenerateKeeperTexture(ILuint TextureDevILID, Sint16 BorderColorID);

    void BindAggregate();
    void BindTexture(Uint32 TextureID);
    void BindTexturePoint(Uint32 TextureID, float u, float v);

    void ReportDevILErrors();
    void ResetTextureBinding();
protected:

	std::vector<ImagePage*> ImageLibrary;
	std::vector<ClipImage*> ClipLibrary;
	std::vector<GLuint> SingularTextureLibrary;
	std::vector<GLuint**> PagedTextureLibrary;

	//std::vector<ILuint> DevilImageVector;
    std::vector<TextureDescriptor> TextureCache;
	std::vector<SDL_Rect> TextureCordinates;

    bool isFileEnding(const char* FilePath, const char* Ending);

    unsigned int currentTexture;
    SDL_Surface* AgragateSurface;
	GLuint MainTexture;
	int MainTextureSize;

};

#define TEXTURE (TextureManager::GetInstance())

#endif // TEXTURE__HEADER
