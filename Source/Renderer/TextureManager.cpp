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

    if (Material->getTexture(SurfaceTypeID) != INVALID_INDEX)
    {
        return Material->getTexture((uint16_t) SurfaceTypeID);
    }
    else
    {
        if (MaterialClassID != INVALID_INDEX)
        {
            int16_t TextureID = DATA->getMaterialClassData(MaterialClassID)->getTexture(SurfaceTypeID);
            if (TextureID == INVALID_INDEX)
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
    if (MaterialID != INVALID_INDEX && TextureID != INVALID_INDEX)
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
            Ogre::MaterialPtr SelectedMaterial = makeStaticMaterial(MaterialID, TextureID);
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

const char* TextureManager::getStaticMaterialName(int16_t MaterialTypeID, int16_t TextureID)
{
    char buffer[32];
    sprintf(buffer, "StaticMat%i-%i",  MaterialTypeID, TextureID);
    return buffer;
}

const char* TextureManager::getStaticTextureName(int16_t MaterialTypeID, int16_t TextureID)
{
    char buffer[32];
    sprintf(buffer, "StaticTex%i-%i",  MaterialTypeID, TextureID);
    return buffer;
}

Ogre::Image* TextureManager::ConvertToOgreImage(int16_t ImageID)
{
    uint8_t* iData = IMAGE->getImageData(ImageID);
    uint16_t Width = IMAGE->getImageWidth(ImageID);
    uint16_t Height = IMAGE->getImageHeight(ImageID);

    Ogre::Image* NewImage = new Ogre::Image(); // Delete?
    NewImage->loadDynamicImage(iData, Width, Height, Ogre::PF_A8R8G8B8);

    return NewImage;
}

Ogre::MaterialPtr TextureManager::makeStaticMaterial(int16_t MaterialTypeID, int16_t TextureID)
{
    uint16_t ImageID = IMAGE->GenerateMaterialImage(MaterialTypeID, TextureID);
    Ogre::Image* NewImage = ConvertToOgreImage(ImageID);

    Ogre::MaterialPtr NewMaterial = Ogre::MaterialManager::getSingleton().create(getStaticMaterialName(MaterialTypeID, TextureID), "General", true);
    Ogre::Technique* FirstTechnique = NewMaterial->getTechnique(0);
    Ogre::Pass* FirstPass = FirstTechnique->getPass(0);

    Ogre::TexturePtr NewTex = Ogre::TextureManager::getSingleton().loadImage(getStaticTextureName(MaterialTypeID, TextureID), "General", *NewImage, Ogre::TEX_TYPE_2D, Ogre::MIP_UNLIMITED, 1.0, true, Ogre::PF_A8R8G8B8);
    //FirstPass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
    Ogre::TextureUnitState* TextureUnit = FirstPass->createTextureUnitState();


    FirstPass->setLightingEnabled(false);
    FirstPass->setAlphaRejectSettings(Ogre::CMPF_GREATER, 0, false);

    //TextureUnit->setTextureFiltering(Ogre::TFO_NONE);
    TextureUnit->setTextureName(getStaticTextureName(MaterialTypeID, TextureID), Ogre::TEX_TYPE_2D);

    return NewMaterial;
}

const char* TextureManager::getAnimationTextureName(int16_t AnimationGroupID, int16_t ColorID, int16_t SlideNumber)
{
    char buffer[64];
    sprintf(buffer, "Slide%i-%i-%i",  AnimationGroupID, ColorID, SlideNumber);
    return buffer;
}

Ogre::MaterialPtr TextureManager::makeAnimatedMaterial(int16_t AnimationGroupID, int16_t ColorID)
{
    AnimationGroupData* Animation = DATA->getAnimationGroupData(AnimationGroupID);
    uint16_t Animations = Animation->getTextureCount();

    Ogre::String* TextureNames = new Ogre::String[Animations];

    Ogre::MaterialPtr NewMaterial = Ogre::MaterialManager::getSingleton().create(getStaticMaterialName(AnimationGroupID, ColorID), "General", true);
    Ogre::Technique* FirstTechnique = NewMaterial->getTechnique(0);
    Ogre::Pass* FirstPass = FirstTechnique->getPass(0);
    //FirstPass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
    FirstPass->setLightingEnabled(false);
    FirstPass->setAlphaRejectSettings(Ogre::CMPF_GREATER, 0, false);


    Ogre::TextureUnitState* TextureUnit = FirstPass->createTextureUnitState();

    TextureUnit->setAnimatedTextureName(TextureNames, Animations, 0);

    for (uint16_t i = 0; i < Animations; i++)
    {
        TextureNames[i] = getAnimationTextureName(AnimationGroupID, ColorID, i);
        TextureData* Texture = DATA->getTextureData(Animation->getTexture(i));

        uint16_t ImageID = IMAGE->GeneratedOverLayImage(Texture->getDevILID(), ColorID, -1);
        Ogre::Image* NewImage = ConvertToOgreImage(ImageID);

        Ogre::TexturePtr NewTex = Ogre::TextureManager::getSingleton().loadImage(TextureNames[i], "General", *NewImage); //, Ogre::TEX_TYPE_2D, Ogre::MIP_UNLIMITED, 1.0, true, Ogre::PF_A8R8G8B8);
     	TextureUnit->setFrameTextureName(TextureNames[i], i);
    }

    TextureUnit->setTextureFiltering(Ogre::TFO_NONE);
    //TextureUnit->setAnimatedTextureName(TextureNames, Animations, 0);

    return NewMaterial;
}
