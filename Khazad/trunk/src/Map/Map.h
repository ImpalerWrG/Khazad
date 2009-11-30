#ifndef MAP__HEADER
#define MAP__HEADER

#include <Singleton.h>
#include <stdafx.h>

#include <Vector3.h>
#include <Coordinates.h>

namespace DFHack
{
    class API;
}

struct Face
{
    inline Face()
    {
        PositiveAxisSurfaceTypeID = -1;
        NegativeAxisSurfaceTypeID = -1;

        MaterialTypeID = -1;
    };

    Sint16 PositiveAxisSurfaceTypeID;
    Sint16 NegativeAxisSurfaceTypeID;

    Sint16 MaterialTypeID;
};


struct t_matglossPair;
class Column;
class Cell;
class Actor;
class Face;
class TreeManager;
union t_occupancy;
struct t_construction;
struct t_tree_desc;
struct t_building;
struct Face;
class DFHackAPI;


class Map
{
DECLARE_SINGLETON_CLASS(Map)

public:

	~Map();
	bool Init();

    bool isInitialized()        { return Initialized; }
    bool isMapLoaded()          { return MapLoaded; }

    uint64_t GenerateCellKey(CellCoordinates KeyCoords) const;
	Cell* getCell(CellCoordinates) const;
	Cell* getCubeOwner(MapCoordinates) const;

    map<uint64_t, Cell*>* getCellMap();
    bool addCell(Cell* NewCell, CellCoordinates TargetCoordinates);

    bool isCubeInited(MapCoordinates) const;
    void setCellNeedsReDraw(CellCoordinates, bool NewValue);

    bool isCubeSloped(MapCoordinates Coordinates) const;

    void setCubeShape(MapCoordinates Coordinates, Sint16 TileShape);
    inline Sint16 getCubeShape(MapCoordinates Coordinates) const;

    void setCubeMaterial(MapCoordinates Coordinates, Sint16 MaterialID);
    inline Sint16 getCubeMaterial(MapCoordinates Coordinates) const;

    void setCubeSurfaceType(MapCoordinates Coordinates, Sint16 SurfaceID);
    inline Sint16 getCubeSurfaceType(MapCoordinates Coordinates) const;

    Face* getFace(MapCoordinates Coordinates, Direction DirectionType) const;
    bool hasFace(MapCoordinates Coordinates, Direction DirectionType) const;

    bool removeFace(MapCoordinates Coordinates, Direction DirectionType);
    Face* addFace(MapCoordinates Coordinates, Direction DirectionType);

    void setFaceMaterial(MapCoordinates Coordinates, Direction DirectionType, Sint16 MaterialID);
    inline Sint16 getFaceMaterial(MapCoordinates Coordinates, Direction DirectionType) const;

    void setFaceSurfaceType(MapCoordinates Coordinates, Direction DirectionType, Sint16 SurfaceID);
    inline Sint16 getFaceSurfaceType(MapCoordinates Coordinates, Direction DirectionType) const;
    void setBothFaceSurfaceTypes(MapCoordinates Coordinates, Direction DirectionType, Sint16 SurfaceID);


    bool isCubeHidden(MapCoordinates Coordinates) const;
    void setCubeHidden(MapCoordinates Coordinates, bool NewValue);

    bool isCubeSubTerranean(MapCoordinates Coordinates) const;
    void setCubeSubTerranean(MapCoordinates Coordinates, bool NewValue);

    bool isCubeSkyView(MapCoordinates Coordinates) const;
    void setCubeSkyView(MapCoordinates Coordinates, bool NewValue);

    bool isCubeSunLit(MapCoordinates Coordinates) const;
    void setCubeSunLit(MapCoordinates Coordinates, bool NewValue);

    bool isCubeSolid(MapCoordinates Coordinates) const;
    void setCubeSolid(MapCoordinates Coordinates, bool NewValue);

    void InitilizeTilePicker(DFHack::API & DF);
    Sint16 PickMaterial(Sint16 TileType, Sint16 basematerial, Sint16 veinmaterial, t_matglossPair constructionmaterial, t_occupancy occupancy);
    Sint16 ResolveMatGlossPair(t_matglossPair MatPair);

    bool Generate(Uint32 Seed);

	Uint32 getMapSizeX() const { return MapSizeX; }
	Uint32 getMapSizeY() const { return MapSizeY; }
	Uint32 getMapSizeZ() const { return MapSizeZ; }

	Uint16 getCellSizeX() const { return CellSizeX; }
	Uint16 getCellSizeY() const { return CellSizeY; }
	Uint16 getCellSizeZ() const { return CellSizeZ; }

    bool Extract();
    bool Load(string filename);
    void Save(string filename);

    void ReleaseMap();

    void DigChannel(MapCoordinates Coordinates);
    void DigSlope(MapCoordinates Coordinates);
    void Dig(MapCoordinates Coordinates);

    void LoadCellData(DFHack::API & context,
                      vector< vector <uint16_t> >& layerassign,
                      Cell* TargetCell,
                      map<uint64_t, t_construction> & constuctions,
                      map<uint64_t, t_tree_desc> & vegetation,
                      map<uint64_t, t_building> & buildings,
                      CellCoordinates NewCellCoordinates);

    void ChangeCellCount(Sint8 Change)      { CellCount += Change; }
    Uint32 getCellCount() const             { return CellCount; }

    void ChangeInitedCellCount(Sint8 Change)      { InitedCellCount += Change; }
    Uint32 getInitedCellCount() const             { return InitedCellCount; }

    void ChangeFaceCount(Sint8 Change)      { FaceCount += Change; }
    Uint32 getFaceCount() const             { return FaceCount; }


    MapCoordinates getMapCenter() const;

    TreeManager* TreeMan;

protected:

    bool Initialized;
    bool MapLoaded;

    Uint32 MapSizeX;
    Uint32 MapSizeY;
    Uint32 MapSizeZ;

    Uint16 CellSizeX;
    Uint16 CellSizeY;
    Uint16 CellSizeZ;

    std::map<uint64_t, Cell*> Cells;

    Sint16 TileShapePicker[600];
    Sint16 TileSurfacePicker[600];
    Sint16 TileMaterialPicker[600];
    Sint16 TileMaterialClassPicker[600];

    vector <int16_t> StoneMatGloss;
    vector <int16_t> MetalMatGloss;
    vector <int16_t> WoodMatGloss;

    Uint32 CellCount;
    Uint32 FaceCount;

    Uint32 InitedCellCount;
};

#define MAP (Map::GetInstance())

#endif // MAP__HEADER
