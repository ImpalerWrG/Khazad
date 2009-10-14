#ifndef TEXTURE__HEADER
#define TEXTURE__HEADER

#include <stdafx.h>

#include <Singleton.h>

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>


class TextureManager
{
	DECLARE_SINGLETON_CLASS(TextureManager)

public:

	~TextureManager();
	bool Init();

    void MergeTextures();

    int round(double x);
    int nextpoweroftwo(int x);

    GLuint GenerateTexture(ILuint DevILImageID);

    void BindTexture(Uint32 TextureID);

    void ReportDevILErrors();
    void ResetTextureBinding();

    GLuint getLogoTexture()                  { return LogoTexture; }
    void setLogoTexture(GLuint NewValue)     { LogoTexture = NewValue; }

    GLuint MapTexture(Sint16 MaterialID, Sint16 SurfaceTypeID);

protected:

    GLuint LogoTexture;

    vector<GLuint> TextureCache;

    map<Uint32, GLuint> TextureMap;

    bool isFileEnding(const char* FilePath, const char* Ending);

    GLuint currentTexture;
};

#define TEXTURE (TextureManager::GetInstance())

#endif // TEXTURE__HEADER
