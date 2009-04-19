#ifndef MAP__HEADER
#define MAP__HEADER

#include <Singleton.h>
#include <stdafx.h>

class Cell;
class Cube;
class Actor;


class Map
{
DECLARE_SINGLETON_CLASS(Map)

public:

	~Map();
	bool Init();

	Cell* getCell(Sint32 X, Sint32 Y, Sint32 Z);
	Cell* getCubeOwner(Sint32 X, Sint32 Y, Sint32 Z);
	Cube* getCube(Sint32 X, Sint32 Y, Sint32 Z);

	//void AddCube(Sint32 X, Sint32 Y, Sint32 Z);
	std::vector<Actor*> Grid;

    bool Generate(Uint32 Seed);

	Uint32 MapSizeX;
	Uint32 MapSizeY;
	Uint32 MapSizeZ;

	Uint32 CellSizeX;
	Uint32 CellSizeY;
	Uint32 CellSizeZ;

    void LoadExtract();

protected:

	Cell**** CellArray;

	Sint32 XCelloffset;
	Sint32 YCelloffset;

	Uint8 CubesPerCellSide;
};

#define MAP (Map::GetInstance())

#endif // MAP__HEADER
