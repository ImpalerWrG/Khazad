#include <Zone.h>

#include <Renderer.h>
#include <Coordinates.h>
#include <TileShapes.h>


Zone::Zone(MapCoordinates SpawnLocation)
{
    OriginLocation = SpawnLocation;
    TerminalLocation = SpawnLocation;

    BoxNode = RENDERER->getRootNode()->createChildSceneNode();
    ManualWireFrame = NULL;
    AxialBox = new Ogre::AxisAlignedBox();
    reBuildAxialBox();
    Active = false;
}

Zone::~Zone()
{
    //RENDERER->getSceneManager()->
    RENDERER->getSceneManager()->destroyManualObject(ManualWireFrame);
}

void Zone::MoveZone(MapCoordinates NewLocation)
{
    OriginLocation = NewLocation;
    //BoxNode->setPosition(Location.X, Location.Y, Location.Z);
}

void Zone::TranslateZone(Direction TranslationDirection, int Distance)
{
    OriginLocation.TranslateMapCoordinates(TranslationDirection, Distance);
    TerminalLocation.TranslateMapCoordinates(TranslationDirection, Distance);

    BoxNode->translate(DirectionToVector(TranslationDirection) * Distance);
}

void Zone::MorphZone(Direction MorphDirection, int ValueChange)
{
    Ogre::Vector3 MorphVector = DirectionToVector(MorphDirection);

    if (isDirectionPositive(MorphDirection))
    {
        Ogre::Vector3 OldMax = AxialBox->getMaximum();
        AxialBox->setMaximum(OldMax + MorphVector);
    }
    else
    {
        Ogre::Vector3 OldMin = AxialBox->getMaximum();
        AxialBox->setMinimum(OldMin + MorphVector);
    }
}

void Zone::Morph2Coordinate(MapCoordinates NewLocation)
{
    TerminalLocation = NewLocation;
    reBuildAxialBox();
}

bool Zone::isCoordinateInZone(MapCoordinates TestCoordinates)
{
    // TODO use MapCoordinates rather then AAB and replicated logic of intersect in integer math
    return AxialBox->intersects(Ogre::Vector3(TestCoordinates.X, TestCoordinates.Y, TestCoordinates.Z));
}

void Zone::reBuildAxialBox()
{
    AxialBox->setMinimumX(min(OriginLocation.X, TerminalLocation.X) - HALFCUBE);
    AxialBox->setMinimumY(min(OriginLocation.Y, TerminalLocation.Y) - HALFCUBE);
    AxialBox->setMinimumZ(min(OriginLocation.Z, TerminalLocation.Z) - HALFCUBE);

    AxialBox->setMaximumX(max(OriginLocation.X, TerminalLocation.X) + HALFCUBE);
    AxialBox->setMaximumY(max(OriginLocation.Y, TerminalLocation.Y) + HALFCUBE);
    AxialBox->setMaximumZ(max(OriginLocation.Z, TerminalLocation.Z) + HALFCUBE);

    if (ManualWireFrame == NULL)
    {
        ManualWireFrame = RENDERER->getSceneManager()->createManualObject();
        WireFrame(false, ManualWireFrame, AxialBox, Ogre::ColourValue (1.0, 1.0, 0.0, 1.0)); // Not an update
        BoxNode->attachObject(ManualWireFrame);
    } else {
        WireFrame(true, ManualWireFrame, AxialBox, Ogre::ColourValue (1.0, 1.0, 0.0, 1.0));  // Updating
    }

    ManualWireFrame->setCastShadows(false);
    BoxNode->setPosition(AxialBox->getMinimum());
}

void Zone::setActive(bool ActiveState)
{
    if (ActiveState != Active)
    {
        Active = ActiveState;

        if (Active)
        {
            BoxNode->setVisible(true);
        }
        else
        {
            BoxNode->setVisible(true);
        }
    }
}
