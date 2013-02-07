#include <Selection.h>

#include <Renderer.h>
#include <Coordinates.h>
#include <TileShapes.h>


VolumeSelection::VolumeSelection(MapCoordinates SpawnLocation)
{
	Zscalar = 0;
    OriginLocation = SpawnLocation;
    TerminalLocation = SpawnLocation;

    BoxNode = RENDERER->getRootNode()->createChildSceneNode();
    ManualWireFrame = NULL;
    AxialBox = new Ogre::AxisAlignedBox();
    reBuildAxialBox();
    Active = false;
}

VolumeSelection::~VolumeSelection()
{
    //RENDERER->getSceneManager()->
    RENDERER->getSceneManager()->destroyManualObject(ManualWireFrame);
}

void VolumeSelection::MoveZone(MapCoordinates NewLocation)
{
    OriginLocation = NewLocation;
    //BoxNode->setPosition(Location.X, Location.Y, Location.Z);
}

void VolumeSelection::TranslateZone(Direction TranslationDirection, int Distance)
{
    OriginLocation.TranslateMapCoordinates(TranslationDirection, Distance);
    TerminalLocation.TranslateMapCoordinates(TranslationDirection, Distance);

    BoxNode->translate(DirectionToVector(TranslationDirection) * Distance);
}

void VolumeSelection::MorphZone(Direction MorphDirection, int ValueChange)
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

void VolumeSelection::Morph2Coordinate(MapCoordinates NewLocation)
{
    TerminalLocation = NewLocation;
    reBuildAxialBox();
}

bool VolumeSelection::isCoordinateInZone(MapCoordinates TestCoordinates)
{
    // TODO use MapCoordinates rather then AAB and replicated logic of intersect in integer math
    return AxialBox->intersects(Ogre::Vector3(TestCoordinates.X, TestCoordinates.Y, TestCoordinates.Z));
}

void VolumeSelection::reBuildAxialBox()
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

void VolumeSelection::setActive(bool ActiveState)
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
            BoxNode->setVisible(false);
        }
    }
}

void VolumeSelection::changeZscalar(float Zchange)
{
	Zscalar += Zchange;
	//int Whole = Zscalar;
	//Zscalar -= Whole;
	TerminalLocation.Z = OriginLocation.Z + Zscalar;
	reBuildAxialBox();
}