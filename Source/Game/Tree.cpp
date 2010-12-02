#include <Tree.h>

#include <DataManager.h>
#include <Map.h>

#include <ImageManager.h>
#include <TextureManager.h>

Tree::Tree()
{
}

Tree::~Tree()
{
    // NOP
}

bool Tree::Init(int16_t TreeType, MapCoordinates SpawnLocation, bool isAlive)
{
    Alive = isAlive;

    TreeData* Data = DATA->getTreeData(TreeType);
    if(Data != NULL)
    {
        int16_t TextureID = Data->getTextureID();
        TextureData* Tex = DATA->getTextureData(TextureID);

        ILuint DevilTexture = IMAGE->GenerateKeeperImage(Tex->getDevILID(), -1);
        Ogre::Image* OgreImage = TEXTURE->ConvertToOgreImage(DevilTexture);


        Ogre::MaterialPtr MatPointer = TEXTURE->makeStaticMaterial(OgreImage, "A Tree");

        Actor::Init(SpawnLocation, MatPointer, 2.0, 3);
        return true;
    }

    return false;
}

CoolDown Tree::Update()
{
    return 1000;
}
