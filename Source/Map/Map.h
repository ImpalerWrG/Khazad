#ifndef MAP__HEADER
#define MAP__HEADER

#include <stdafx.h>

#include <Coordinates.h>
#include <Renderer.h>


class Cell;
class Geology;
class Face;
class Zone;


class Map
{

public:

    Map();

	~Map();
	bool Init();

    bool isInitialized()        { return Initialized; }
    bool isMapLoaded()          { return MapLoaded; }

    uint64_t GenerateCellKey(CellCoordinates KeyCoords) const;
	Cell* getCell(CellCoordinates) const;
	Cell* getCubeOwner(MapCoordinates) const;

    std::map<uint64_t, Cell*>* getCellMap();
    bool addCell(Cell* NewCell, CellCoordinates TargetCoordinates);

    bool isCubeInited(MapCoordinates) const;
    void setCellNeedsReDraw(CellCoordinates, bool NewValue);

    bool isCubeSloped(MapCoordinates Coordinates) const;

    void setCubeShape(MapCoordinates Coordinates, int16_t TileShape);
    int16_t getCubeShape(MapCoordinates Coordinates) const;

    void setCubeMaterial(MapCoordinates Coordinates, int16_t MaterialID);
    inline int16_t getCubeMaterial(MapCoordinates Coordinates) const;

    void setCubeSurfaceType(MapCoordinates Coordinates, int16_t SurfaceID);
    inline int16_t getCubeSurfaceType(MapCoordinates Coordinates) const;

    Face* getFace(MapCoordinates Coordinates, Direction DirectionType) const;
    bool hasFace(MapCoordinates Coordinates, Direction DirectionType) const;

    bool removeFace(MapCoordinates Coordinates, Direction DirectionType);
    Face* addFace(MapCoordinates Coordinates, Direction DirectionType);

    void setFaceMaterial(MapCoordinates Coordinates, Direction DirectionType, int16_t MaterialID);
    inline int16_t getFaceMaterial(MapCoordinates Coordinates, Direction DirectionType) const;

    void setFaceSurfaceType(MapCoordinates Coordinates, Direction DirectionType, int16_t SurfaceID);
    inline int16_t getFaceSurfaceType(MapCoordinates Coordinates, Direction DirectionType) const;
    void setBothFaceSurfaceTypes(MapCoordinates Coordinates, Direction DirectionType, int16_t SurfaceID);


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


    bool Generate(Geology* RegionGeology);

	uint32_t getMapSizeX() const { return MapSizeX; }
	uint32_t getMapSizeY() const { return MapSizeY; }
	uint32_t getMapSizeZ() const { return MapSizeZ; }

	uint16_t getCellSizeX() const { return CellSizeX; }
	uint16_t getCellSizeY() const { return CellSizeY; }
	uint16_t getCellSizeZ() const { return CellSizeZ; }

    bool Load(std::string filename);
    void Save(std::string filename);

    void ReleaseMap();

    void DigChannel(MapCoordinates Coordinates);
    void DigSlope(MapCoordinates Coordinates);
    void Dig(MapCoordinates Coordinates);

    uint32_t getCellCount() const             { return CellCount; }
    uint32_t getFaceCount() const             { return FaceCount; }


    MapCoordinates getMapCenter() const;

    Zone* getZone() const;
    void addZone(Zone* NewZone);
    void setActiveZone(Zone* ActivatedZone)        { ActiveZone = ActivatedZone; }
    Zone* getActiveZone()                          { return ActiveZone; }
    void DeactivateZone()                          { ActiveZone = NULL; }

    Zone* getZoneAt(MapCoordinates TestCoordinates);

protected:

    bool Initialized;
    bool MapLoaded;

    uint32_t MapSizeX;
    uint32_t MapSizeY;
    uint32_t MapSizeZ;

    uint16_t CellSizeX;
    uint16_t CellSizeY;
    uint16_t CellSizeZ;

    std::map<uint64_t, Cell*> Cells;
    std::vector<Zone*> Zones;

    Zone* ActiveZone;

    uint32_t CellCount;
    uint32_t FaceCount;

    Geology* MapGeology;
};

#endif // MAP__HEADER
