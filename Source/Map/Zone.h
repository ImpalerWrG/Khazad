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

    bool isCoordinateInZone(MapCoordinates TestCoordinates);

    void setActive(bool ActiveState);

    MapCoordinates getLocation()        { return Location; }

private:

    // Multiple Boxes for irregular areas, custome wireframes for cleaner apearance?
    MapCoordinates Location;

    Ogre::AxisAlignedBox AxialBox;
    Ogre::SceneNode* BoxNode;

    bool Active;

    /* Redesign
    std::vector< Cell* > Cells  // Pointer to each Cell in which the Zone exists

    std::vector< bitset< CUBESPERCELL > > isZone;  // parrellel vector of booleans flagging the individual Cubes in Cells as part of Zone

    when expanding a zone, the Map::isZone flag is set

    */
};

#endif // ZONE__HEADER
