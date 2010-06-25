#include <Geology.h>

#include <DataManager.h>


Geology::Geology()
{

}

Geology::~Geology()
{

}

bool Geology::Init(uint32_t Seed)
{
    Ogre::Plane RockLayer = Ogre::Plane(Ogre::Vector3(0, 0, 1), 1);
    int RockType = 42;

    LayerBoundries.push_back(make_pair(RockLayer, RockType));


    return true;
}
