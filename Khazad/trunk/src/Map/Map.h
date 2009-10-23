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
struct Face;
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

    bool isCubeSloped(Sint32 MapX, Sint32 MapY, Sint32 MapZ);

    void setCubeShape(Sint32 MapX, Sint32 MapY, Sint32 MapZ, Sint16 TileShape);
    inline Sint16 getCubeShape(Sint32 MapX, Sint32 MapY, Sint32 MapZ);

    void setCubeMaterial(Sint32 MapX, Sint32 MapY, Sint32 MapZ, Sint16 MaterialID);
    inline Sint16 getCubeMaterial(Sint32 MapX, Sint32 MapY, Sint32 MapZ);

    void setCubeSurfaceType(Sint32 MapX, Sint32 MapY, Sint32 MapZ, Sint16 SurfaceID);
    inline Sint16 getCubeSurfaceType(Sint32 MapX, Sint32 MapY, Sint32 MapZ);



    Face* getFace(Sint32 MapX, Sint32 MapY, Sint32 MapZ, Facet FacetType);
    bool hasFace(Sint32 MapX, Sint32 MapY, Sint32 MapZ, Facet FacetType);

    bool removeFace(Sint32 MapX, Sint32 MapY, Sint32 MapZ, Facet FacetType);
    Face* addFace(Sint32 MapX, Sint32 MapY, Sint32 MapZ, Facet FacetType);

    void setFaceMaterial(Sint32 MapX, Sint32 MapY, Sint32 MapZ, Facet FacetType, Sint16 MaterialID);
    inline Sint16 getFaceMaterial(Sint32 MapX, Sint32 MapY, Sint32 MapZ, Facet FacetType);

    void setFaceSurfaceType(Sint32 MapX, Sint32 MapY, Sint32 MapZ, Facet FacetType, Sint16 SurfaceID);
    inline Sint16 getFaceSurfaceType(Sint32 MapX, Sint32 MapY, Sint32 MapZ, Facet FacetType);



    bool isCubeHidden(Sint32 MapX, Sint32 MapY, Sint32 MapZ);
    void setCubeHidden(Sint32 MapX, Sint32 MapY, Sint32 MapZ, bool NewValue);

    bool isCubeSubTerranean(Sint32 MapX, Sint32 MapY, Sint32 MapZ);
    void setCubeSubTerranean(Sint32 MapX, Sint32 MapY, Sint32 MapZ, bool NewValue);

    bool isCubeSkyView(Sint32 MapX, Sint32 MapY, Sint32 MapZ);
    void setCubeSkyView(Sint32 MapX, Sint32 MapY, Sint32 MapZ, bool NewValue);

    bool isCubeSunLit(Sint32 MapX, Sint32 MapY, Sint32 MapZ);
    void setCubeSunLit(Sint32 MapX, Sint32 MapY, Sint32 MapZ, bool NewValue);

    bool isCubeSolid(Sint32 MapX, Sint32 MapY, Sint32 MapZ);
    void setCubeSolid(Sint32 MapX, Sint32 MapY, Sint32 MapZ, bool NewValue);



    void InitilizeTilePicker(DFHackAPI & DF);
    Sint16 PickMaterial(Sint16 TileType, Sint16 basematerial, Sint16 veinmaterial,t_matglossPair constructionmaterial, t_occupancy occupancy);

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

    void ChangeFaceCount(Sint8 Change)      { FaceCount += Change; }
    Uint32 getFaceCount()                   { return FaceCount; }


    Vector3 getMapCenter();

    TreeManager* TreeMan;

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

    Sint16 TileShapePicker[600];
    Sint16 TileSurfacePicker[600];
    Sint16 TileMaterialPicker[600];
    Sint16 TileMaterialClassPicker[600];

    vector <int16_t> StoneMatGloss;
    vector <int16_t> MetalMatGloss;

    Uint32 CellCount;
    Uint32 FaceCount;

    Uint32 InitedCellCount;
};

#define MAP (Map::GetInstance())

#endif // MAP__HEADER
