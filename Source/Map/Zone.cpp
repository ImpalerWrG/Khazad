#include <Zone.h>

#include <Renderer.h>
#include <Coordinates.h>


Zone::Zone(MapCoordinates SpawnLocation)
{
    Location = SpawnLocation;

    Ogre::Vector3 Min = Ogre::Vector3(Location.X - HALFCUBE, Location.Y - HALFCUBE, Location.Z - HALFCUBE);
    Ogre::Vector3 Max = Ogre::Vector3(Location.X + HALFCUBE, Location.Y + HALFCUBE, Location.Z + HALFCUBE);

    AxialBox = Ogre::AxisAlignedBox(Min, Max);
    Active = false;

    BoxNode = RENDERER->getRootNode()->createChildSceneNode();

    Ogre::Entity* WireBox = RENDERER->getSceneManager()->createEntity("WhiteWireFrame");
    WireBox->setCastShadows(false);
    BoxNode->attachObject(WireBox);

    BoxNode->setPosition(SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z);
}

Zone::~Zone()
{
    delete BoxNode;
}

void Zone::MoveZone(MapCoordinates NewLocation)
{
    Location = NewLocation;
    BoxNode->setPosition(Location.X, Location.Y, Location.Z);
}

void Zone::TranslateZone(Direction TranslationDirection, int Distance)
{
    Location.TranslateMapCoordinates(TranslationDirection, Distance);

    BoxNode->translate(DirectionToVector(TranslationDirection) * Distance);
}

void Zone::MorphZone(Direction MorphDirection, int ValueChange)
{
    Ogre::Vector3 MorphVector = DirectionToVector(MorphDirection);

    if (isDirectionPositive(MorphDirection))
    {
        Ogre::Vector3 OldMax = AxialBox.getMaximum();
        AxialBox.setMaximum(OldMax + MorphVector);
    }
    else
    {
        Ogre::Vector3 OldMin = AxialBox.getMaximum();
        AxialBox.setMinimum(OldMin + MorphVector);
    }
}

bool Zone::isCoordinateInZone(MapCoordinates TestCoordinates)
{
    // TODO use MapCoordinates rather then AAB and replicated logic of intersect in integer math
    return AxialBox.intersects(Ogre::Vector3(TestCoordinates.X, TestCoordinates.Y, TestCoordinates.Z));
}

void Zone::setActive(bool ActiveState)
{
    if (ActiveState != Active)
    {
        Active = ActiveState;

        if (Active)
        {
            //BoxNode->detachObject(WireBox);
            BoxNode->detachAllObjects();

            Ogre::Entity* WireBox = RENDERER->getSceneManager()->createEntity("YellowWireFrame");
            WireBox->setCastShadows(false);
            BoxNode->attachObject(WireBox);
        }
        else
        {
            //BoxNode->detachObject(WireBox);
            BoxNode->detachAllObjects();

            Ogre::Entity* WireBox = RENDERER->getSceneManager()->createEntity("WhiteWireFrame");
            WireBox->setCastShadows(false);
            BoxNode->attachObject(WireBox);
        }
    }
}
