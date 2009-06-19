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

    bool isInitialized()        { return Initialized; }
    bool isMapLoaded()          { return MapLoaded; }

	Cell* getCell(Sint32 X, Sint32 Y, Sint32 Z);
	Cell* getCubeOwner(Sint32 X, Sint32 Y, Sint32 Z);
	Cube* getCube(Sint32 X, Sint32 Y, Sint32 Z);
    Face* getFace(Sint32 X, Sint32 Y, Sint32 Z, Facet FaceType);

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

    void LoadCubeData(Cell* TargetCell, Uint32 CellX, Uint32 CellY, Uint32 CellZ, Uint32 CubeX, Uint32 CubeY);

    void ChangeCellCount(Sint8 Change)      { CellCount += Change; }
    Uint32 getCellCount()                   { return CellCount; }

    void ChangeInitedCellCount(Sint8 Change)      { InitedCellCount += Change; }
    Uint32 getInitedCellCount()                   { return InitedCellCount; }

    void ChangeCubeCount(Sint8 Change)      { CubeCount += Change; }
    Uint32 getCubeCount()                   { return CubeCount; }

    void ChangeInitedCubeCount(Sint8 Change)      { InitedCubeCount += Change; }
    Uint32 getInitedCubeCount()                   { return InitedCubeCount; }

    void ChangeFaceCount(Sint8 Change)      { FaceCount += Change; }
    Uint32 getFaceCount()                   { return FaceCount; }

    void ChangeInitedFaceCount(Sint8 Change)      { InitedFaceCount += Change; }
    Uint32 getInitedFaceCount()                   { return InitedFaceCount; }

    void ChangeSlopeCount(Sint8 Change)     { SlopeCount += Change; }
    Uint32 getSlopeCount()                  { return SlopeCount; }

    void ChangeInitedSlopeCount(Sint8 Change)     { InitedSlopeCount += Change; }
    Uint32 getInitedSlopeCount()                  { return InitedSlopeCount; }

protected:

    bool Initialized;
    bool MapLoaded;

	Uint32 MapSizeX;
	Uint32 MapSizeY;
	Uint32 MapSizeZ;

	Uint32 CellSizeX;
	Uint32 CellSizeY;
	Uint32 CellSizeZ;

	Cell**** CellArray;

	//Sint32 XCelloffset;
	//Sint32 YCelloffset;

	Uint16 TilePicker[600];

	Uint32 CellCount;
    Uint32 CubeCount;
    Uint32 FaceCount;
    Uint32 SlopeCount;

	Uint32 InitedCellCount;
    Uint32 InitedCubeCount;
    Uint32 InitedFaceCount;
    Uint32 InitedSlopeCount;

};

#define MAP (Map::GetInstance())

#endif // MAP__HEADER
