#include <stdafx.h>

#include <TextureManager.h>

#include <DataManager.h>
#include <ImageManager.h>



DECLARE_SINGLETON(TextureManager)

TextureManager::TextureManager()
{

}

TextureManager::~TextureManager()
{

}

bool TextureManager::Init()
{
    //glEnable(GL_TEXTURE_2D);

    //setLogoTexture(GenerateTexture(DATA->getTextureData(DATA->getLabelIndex("TEXTURE_KHAZAD_LOGO"))->getDevILID()));

	return true;
}

int16_t TextureManager::PickImageTexture(int16_t MaterialID, int16_t SurfaceTypeID)
{
    MaterialData* Material = DATA->getMaterialData(MaterialID);
    int16_t MaterialClassID = Material->getMaterialClass();

    if (Material->getTexture(SurfaceTypeID) != -1)
    {
        return Material->getTexture((uint16_t) SurfaceTypeID);
    }
    else
    {
        if (MaterialClassID != -1)
        {
            int16_t TextureID = DATA->getMaterialClassData(MaterialClassID)->getTexture(SurfaceTypeID);
            if (TextureID == -1)
            {
                cerr << "bad material/surface combination, no texture. MaterialClassID: " << MaterialClassID << " SurfaceTypeID: " << SurfaceTypeID << endl;
                return  DATA->getLabelIndex("TEXTURE_NEHE");
            }
            else
            {
                return TextureID;
            }
        }
        return  DATA->getLabelIndex("TEXTURE_NEHE");
    }
}

Ogre::MaterialPtr TextureManager::MapTexture(int16_t MaterialID, int16_t TextureID)
{
    if (MaterialID != -1 && TextureID != -1)
    {
        uint32_t Key = MaterialID;
        Key = Key << 16;
        Key += TextureID;

        if (TextureMap.find(Key) != TextureMap.end())
        {
            return TextureMap.find(Key)->second;
        }
        else
        {
            Ogre::MaterialPtr SelectedMaterial = makeOgreMaterial(MaterialID, TextureID);
            if (SelectedMaterial.get() != NULL)
            {
                TextureMap[Key] = SelectedMaterial;
                return SelectedMaterial;
            }
        }
    }
    return Ogre::MaterialPtr(NULL);
}

Ogre::MaterialPtr TextureManager::getOgreMaterial(int16_t MaterialTypeID, int16_t SurfaceTypeID)
{
    int16_t TextureID = PickImageTexture(MaterialTypeID, SurfaceTypeID);

    return MapTexture(MaterialTypeID, TextureID);
}

Ogre::MaterialPtr TextureManager::makeOgreMaterial(int16_t MaterialTypeID, int16_t TextureID)
{
    Ogre::MaterialPtr NewMaterial = Ogre::MaterialManager::getSingleton().create("GrassMaterial", "General", true);

    Ogre::Image* NewImage = new Ogre::Image(); // Delete?
    uint16_t ImageID = IMAGE->GenerateMaterialImage(MaterialTypeID, TextureID);
    uint8_t* iData = IMAGE->getImageData(ImageID);
    uint16_t Width = IMAGE->getImageWidth(ImageID);
    uint16_t Height = IMAGE->getImageHeight(ImageID);


    NewImage->loadDynamicImage(iData, Width, Height, Ogre::PF_A8R8G8B8);
    Ogre::TexturePtr NewTex = Ogre::TextureManager::getSingleton().loadImage("TextureX", "General", *NewImage, Ogre::TEX_TYPE_2D, 1);

    Ogre::Technique* FirstTechnique = NewMaterial->getTechnique(0);
    Ogre::Pass* FirstPass = FirstTechnique->getPass(0);
    FirstPass->setLightingEnabled(false);

    Ogre::TextureUnitState* TextureUnit = FirstPass->createTextureUnitState();
    TextureUnit->setTextureName("TextureX", Ogre::TEX_TYPE_2D);

    delete NewImage;

    return NewMaterial;
}
