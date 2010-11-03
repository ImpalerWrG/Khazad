#include <Zone.h>

#include <Renderer.h>
#include <Coordinates.h>


Zone::Zone(MapCoordinates SpawnLocation)
{
    Ogre::Vector3 Min = Ogre::Vector3(SpawnLocation.X - HALFCUBE, SpawnLocation.Y - HALFCUBE, SpawnLocation.Z - HALFCUBE);
    Ogre::Vector3 Max = Ogre::Vector3(SpawnLocation.X + HALFCUBE, SpawnLocation.Y + HALFCUBE, SpawnLocation.Z + HALFCUBE);

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
    BoxNode->setPosition(NewLocation.X, NewLocation.Y, NewLocation.Z);
}

void Zone::TranslateZone(Direction TranslationDirection, int Distance)
{
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
