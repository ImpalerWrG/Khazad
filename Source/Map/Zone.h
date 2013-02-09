#ifndef ZONE__HEADER
#define ZONE__HEADER

#include <stdafx.h>

#include <Coordinates.h>

class VolumeSelection;

class Zone
{

public:

	Zone(std::vector<VolumeSelection*> Volumes);
	~Zone();

	void addSelection(VolumeSelection* Selection);
	void ModifyZone(std::vector<VolumeSelection> Volumes);

    bool isCoordinateInZone(MapCoordinates TestCoordinates);

private:

    // Multiple Boxes for irregular areas, custome wireframes for cleaner apearance?
    //Ogre::ManualObject* ManualWireFrame;

    bool Active;

	std::map< uint64_t, bitset <CUBESPERCELL>* > ZoneMap;

    std::vector< CellCoordinates > Cells; 			// Coordinates of each Cell in which the Zone exists
    std::vector< bitset <CUBESPERCELL> > isZone;	// parrellel vector of booleans flagging the individual Cubes in Cells as part of Zone
};

#endif // ZONE__HEADER
