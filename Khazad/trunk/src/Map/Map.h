#ifndef MAP__HEADER
#define MAP__HEADER

#include <Singleton.h>
#include <stdafx.h>

#include <Vector3.h>

struct t_matglossPair;
class Column;
class Cell;
class Cube;
class Actor;
class Face;
class TreeManager;
union t_occupancy;
struct t_construction;
struct t_tree_desc;
struct t_building;
class DFHackAPI;
//class Extractor;
//class DfMap;

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

    bool hasFace(Sint32 X, Sint32 Y, Sint32 Z, Facet FaceType);

    void InitilizeTilePicker(DFHackAPI & DF);
    Uint32 PickTexture(Sint16 TileType, Sint16 basematerial, Sint16 veinmaterial,t_matglossPair constructionmaterial, t_occupancy occupancy);

    void BuildVertexArray();

    bool Generate(Uint32 Seed);

	Uint32 getMapSizeX() { return MapSizeX; }
	Uint32 getMapSizeY() { return MapSizeY; }
	Uint32 getMapSizeZ() { return MapSizeZ; }

	Uint32 getCellSizeX() { return CellSizeX; }
	Uint32 getCellSizeY() { return CellSizeY; }
	Uint32 getCellSizeZ() { return CellSizeZ; }

    bool Extract();
    bool Load(string filename);
    void Save(string filename);

    void ReleaseMap();

    void LoadCellData(DFHackAPI & context,
                      vector< vector <uint16_t> >& layerassign,
                      Cell* TargetCell,
                      map<uint64_t, t_construction> & constuctions,
                      map<uint64_t, t_tree_desc> & vegetation,
                      map<uint64_t, t_building> & buildings,
                      Uint32 CellX, Uint32 CellY, Uint32 CellZ);

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

    Vector3 getMapCenter();

    TreeManager * TreeMan;

protected:

    bool Initialized;
    bool MapLoaded;

    Uint32 MapSizeX;
    Uint32 MapSizeY;
    Uint32 MapSizeZ;

    Uint32 CellSizeX;
    Uint32 CellSizeY;
    Uint32 CellSizeZ;

    Column*** ColumnMatrix;

    Uint16 TilePicker[600];
    Uint16 TileShapePicker[600];

    vector <int16_t> StoneMatGloss;

    Uint32 CellCount;
    Uint32 CubeCount;
    Uint32 FaceCount;
    Uint32 SlopeCount;

    Uint32 InitedCellCount;
    Uint32 InitedCubeCount;
    Uint32 InitedFaceCount;
    Uint32 InitedSlopeCount;

    float* VertexArray;
};

#define MAP (Map::GetInstance())

#endif // MAP__HEADER
