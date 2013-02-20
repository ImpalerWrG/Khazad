#include <Selection.h>

#include <Renderer.h>
#include <Coordinates.h>
#include <TileShapes.h>
#include <WireFrame.h>


VolumeSelection::VolumeSelection(MapCoordinates SpawnLocation)
{
	Zscalar = 0;
    OriginLocation = SpawnLocation;
    TerminalLocation = SpawnLocation;

    WireFrameRender = new WireFrame(SpawnLocation);
    Active = false;
}

VolumeSelection::~VolumeSelection()
{
	delete WireFrameRender;
}

void VolumeSelection::MoveZone(MapCoordinates NewLocation)
{
    OriginLocation = NewLocation;
}

void VolumeSelection::TranslateZone(Direction TranslationDirection, int Distance)
{

}

void VolumeSelection::MorphZone(Direction MorphDirection, int ValueChange)
{

}

void VolumeSelection::Morph2Coordinate(MapCoordinates NewLocation)
{
    TerminalLocation = NewLocation;
    WireFrameRender->setShape(OriginLocation, TerminalLocation, true);
}

bool VolumeSelection::isCoordinateInZone(MapCoordinates TestCoordinates)
{
    // TODO use MapCoordinates rather then AAB and replicated logic of intersect in integer math
    //return AxialBox->intersects(Ogre::Vector3(TestCoordinates.X, TestCoordinates.Y, TestCoordinates.Z));
}

void VolumeSelection::setActive(bool ActiveState)
{
    if (ActiveState != Active)
    {
        Active = ActiveState;

        if (Active)
        {
            WireFrameRender->setVisible(true);
        }
        else
        {
            WireFrameRender->setVisible(false);
        }
    }
}

void VolumeSelection::changeZscalar(float Zchange)
{
	Zscalar += Zchange;
	TerminalLocation.Z = OriginLocation.Z + Zscalar;
	WireFrameRender->setShape(OriginLocation, TerminalLocation, true);
}
