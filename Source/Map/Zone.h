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

private:

    // Multiple Boxes for irregular areas, custome wireframes for cleaner apearance?

    Ogre::AxisAlignedBox AxialBox;
    Ogre::SceneNode* BoxNode;
};

#endif // ZONE__HEADER
