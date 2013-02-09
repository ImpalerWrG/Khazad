#ifndef SELECTION__HEADER
#define SELECTION__HEADER

#include <stdafx.h>

#include <Coordinates.h>
#include <Ogre.h>


class VolumeSelection
{
friend class Zone;

public:

	VolumeSelection(MapCoordinates SpawnLocation);
	~VolumeSelection();

    void MoveZone(MapCoordinates NewLocation);
    void TranslateZone(Direction TranslationDirection, int Distance);
    void MorphZone(Direction MorphDirection, int ValueChange);
    void Morph2Coordinate(MapCoordinates NewLocation);
    void reBuildAxialBox();
    bool isCoordinateInZone(MapCoordinates TestCoordinates);

    void setActive(bool ActiveState);
    void changeZscalar(float Zchange);

    MapCoordinates getOriginLocation()        { return OriginLocation; }

private:

    MapCoordinates OriginLocation;
    MapCoordinates TerminalLocation;

    Ogre::AxisAlignedBox* AxialBox;
    Ogre::SceneNode* BoxNode;
    Ogre::ManualObject* ManualWireFrame;

    bool Active;

	float Zscalar;
};

#endif // SELECTION__HEADER
