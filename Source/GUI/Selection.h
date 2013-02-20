#ifndef SELECTION__HEADER
#define SELECTION__HEADER

#include <stdafx.h>

#include <Coordinates.h>

class WireFrame;

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
    bool isCoordinateInZone(MapCoordinates TestCoordinates);

    void setActive(bool ActiveState);
    void changeZscalar(float Zchange);

    MapCoordinates getOriginLocation()        { return OriginLocation; }

private:

    MapCoordinates OriginLocation;
    MapCoordinates TerminalLocation;

    WireFrame* WireFrameRender;

    bool Active;

	float Zscalar;
};

#endif // SELECTION__HEADER
