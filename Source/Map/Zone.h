#ifndef ZONE__HEADER
#define ZONE__HEADER

#include <stdafx.h>

#include <Coordinates.h>
#include <Ogre.h>


class Zone
{
public:

	Zone(MapCoordinates SpawnLocation);
	~Zone();

    void MoveZone(MapCoordinates NewLocation);
    void TranslateZone(Direction TranslationDirection, int Distance);
    void MorphZone(Direction MorphDirection, int ValueChange);
    void Morph2Coordinate(MapCoordinates NewLocation);
    void reBuildAxialBox();
    bool isCoordinateInZone(MapCoordinates TestCoordinates);

    void setActive(bool ActiveState);
    void changeZscalar(float Zchange);

    MapCoordinates getLocation()        { return OriginLocation; }

private:

    // Multiple Boxes for irregular areas, custome wireframes for cleaner apearance?
    MapCoordinates OriginLocation;
    MapCoordinates TerminalLocation;

    Ogre::AxisAlignedBox* AxialBox;
    Ogre::SceneNode* BoxNode;
    Ogre::ManualObject* ManualWireFrame;

    bool Active;

	float Zscalar;
    /* Redesign
    std::vector< Cell* > Cells  // Pointer to each Cell in which the Zone exists

    std::vector< bitset< CUBESPERCELL > > isZone;  // parrellel vector of booleans flagging the individual Cubes in Cells as part of Zone

    when expanding a zone, the Map::isZone flag is set

    */
};

#endif // ZONE__HEADER
