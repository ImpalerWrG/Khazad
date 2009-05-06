#ifndef MAP__HEADER
#define MAP__HEADER

#include <Singleton.h>
#include <stdafx.h>
#include <Cell.h>

//class Cell;
class Cube;
class Actor;


class Map
{
DECLARE_SINGLETON_CLASS(Map)

public:

	~Map();
	bool Init();

    bool Initialized;

	Cell* getCell(Sint32 X, Sint32 Y, Sint32 Z);
	Cell* getCubeOwner(Sint32 X, Sint32 Y, Sint32 Z);
	Cube* getCube(Sint32 X, Sint32 Y, Sint32 Z);

    void InitilizeTilePicker();
    Uint32 PickTexture(int TileType);

    bool Generate(Uint32 Seed);

	Uint32 getMapSizeX() { return MapSizeX; }
	Uint32 getMapSizeY() { return MapSizeY; }
	Uint32 getMapSizeZ() { return MapSizeZ; }

	Uint32 getCellSizeX() { return CellSizeX; }
	Uint32 getCellSizeY() { return CellSizeY; }
	Uint32 getCellSizeZ() { return CellSizeZ; }

    void LoadExtract();


protected:


	Uint32 MapSizeX;
	Uint32 MapSizeY;
	Uint32 MapSizeZ;

	Uint32 CellSizeX;
	Uint32 CellSizeY;
	Uint32 CellSizeZ;

	Cell**** CellArray;

	Sint32 XCelloffset;
	Sint32 YCelloffset;

	Uint8 CubesPerCellSide;

	Uint16 TilePicker[600];
};

#define MAP (Map::GetInstance())

#endif // MAP__HEADER
