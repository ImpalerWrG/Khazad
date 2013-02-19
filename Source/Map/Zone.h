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

	std::map< uint64_t, bitset <CUBESPERCELL>* > ZoneMap;
};

#endif // ZONE__HEADER
