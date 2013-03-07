#include <Zone.h>

#include <Renderer.h>
#include <Coordinates.h>
#include <Selection.h>

Zone::Zone(std::vector<VolumeSelection*> Volumes)
{
	for (int i =0; i < Volumes.size(); i++)
	{
		addSelection(Volumes[i]);
	}
}

Zone::~Zone()
{
	for (std::map< uint64_t, bitset <CUBESPERCELL>* >::iterator it = ZoneMap.begin(); it != ZoneMap.end(); it++)
	{
		delete it->second;
	}
}

void Zone::addSelection(VolumeSelection* Selection)
{
	MapCoordinates Origin = Selection->OriginLocation;
	MapCoordinates Terminal = Selection->TerminalLocation;

	for (int x = Origin.X; x < Terminal.X; x++)
	{
		for (int y = Origin.Y; y < Terminal.Y; y++)
		{
			for (int z = Origin.Z; z < Terminal.Z; z++)
			{
				MapCoordinates ZoneCube = MapCoordinates(x, y, z);
				CellCoordinates ZoneCell = CellCoordinates(ZoneCube);

				std::map< uint64_t, bitset <CUBESPERCELL>* >::iterator it = ZoneMap.find(ZoneCell.Key());
				if (it != ZoneMap.end())
				{
					bitset<CUBESPERCELL>* Bits = it->second;
					Bits->set(ZoneCube.Cube(), true);
				} else {
					bitset<CUBESPERCELL>* Bits = new bitset<CUBESPERCELL>;
					Bits->set(ZoneCube.Cube(), true);
					ZoneMap[ZoneCell.Key()] = Bits;
				}
			}
		}
	}
}

bool Zone::isCoordinateInZone(MapCoordinates TestCoordinates)
{
	std::map< uint64_t, bitset <CUBESPERCELL>* >::iterator it = ZoneMap.find(CellCoordinates(TestCoordinates).Key());
	if (it != ZoneMap.end())
	{
		return it->second->test(TestCoordinates.Cube());
	} else {
		return false;
	}
}
