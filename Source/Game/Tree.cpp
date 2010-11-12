#include <Tree.h>

#include <DataManager.h>
#include <Map.h>


Tree::Tree()
{
    Alive = isAlive;
}

Tree::~Tree()
{
    // NOP
}

bool Tree::Init(int16_t TreeType, MapCoordinates SpawnLocation, bool isAlive)
{
    setLocation(SpawnLocation);

    TreeData* Data = DATA->getTreeData(TreeType);
    if(Data != NULL)
    {

    }




    Ogre::MaterialPtr MatPointer = TEXTURE->makeAnimatedMaterial(DATA->getLabelIndex("ANIMATION_HUMAN"), DATA->getLabelIndex("COLOR_BROWN"));

    Actor::Init(SpawnLocation, MatPointer, 2.0, 2.0);

    return true;
}

CoolDown Tree::Update()
{
    return 1000;
}
