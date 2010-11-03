#ifndef TEXTURE__HEADER
#define TEXTURE__HEADER

#include <stdafx.h>

#include <Singleton.h>

#include <Ogre.h>

class TextureManager
{
	DECLARE_SINGLETON_CLASS(TextureManager)

public:

	~TextureManager();
	bool Init();

    int16_t PickImageTexture(int16_t MaterialID, int16_t SurfaceTypeID);
    Ogre::MaterialPtr MapTexture(int16_t MaterialID, int16_t TextureID);

    const char* getMaterialName(int16_t MaterialTypeID, int16_t TextureID);
    const char* getTextureName(int16_t MaterialTypeID, int16_t TextureID);

    Ogre::MaterialPtr getOgreMaterial(int16_t MaterialTypeID, int16_t SurfaceTypeID);
    Ogre::MaterialPtr makeOgreMaterial(int16_t MaterialTypeID, int16_t TextureID);

protected:

    map<uint32_t, Ogre::MaterialPtr> TextureMap;

};

#define TEXTURE (TextureManager::GetInstance())

#endif // TEXTURE__HEADER
