#include <Geology.h>

#include <DataManager.h>
#include <Random.h>

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

    RandGenerator = new RandomNumberGenerator();
    RandGenerator->Init();

    LayerBoundries.push_back(make_pair(RockLayer, RockType));

    return true;
}

int16_t Geology::getRockTypeAtCoordinates(MapCoordinates Target)
{
    static uint16_t RockType0 = DATA->getLabelIndex("MATERIAL_GRANITE");
    static uint16_t RockType1 = DATA->getLabelIndex("MATERIAL_SANDSTONE");
    static uint16_t RockType2 = DATA->getLabelIndex("MATERIAL_LIMESTONE");

    if (RandGenerator->Roll(0, 3) < Target.Z)
    {
        return INVALID_INDEX;
    }

    if (Target.Z == 3)
    {
        return INVALID_INDEX;
    }
    if (Target.Z == 2)
    {
        return RockType2;
    }
    if (Target.Z == 1)
    {
        return RockType1;
    }
    if (Target.Z == 0)
    {
        return RockType0;
    }
    return RockType0;
}
