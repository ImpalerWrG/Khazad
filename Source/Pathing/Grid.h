#ifndef GRID_HEADER
#define GRID_HEADER

#include <vector>
#include <stdint.h>
#include <map>
#include <string.h>
#include <Coordinates.h>

class Map;

typedef uint32_t DirectionFlags;  // Bitfield for all directions

struct gridInterface  // Virtural Base class
{
    virtual ~gridInterface() {};

    virtual float getEdgeCost(const MapCoordinates &TestCoords, Direction DirectionType) const = 0;

    virtual int max(Axis AxialComponent) const = 0;
    virtual int min(Axis AxialComponent) const = 0;

    virtual DirectionFlags getDirectionFlags(const MapCoordinates &TargetCoords) const = 0;
    virtual bool isPathPossible(const MapCoordinates StartCoords, const MapCoordinates GoalCoords) = 0;

    virtual bool contains(const MapCoordinates &TestCoords) const = 0;
};


struct GridCell
{
    GridCell(CellCoordinates Coordinates)
    {
        thisCellCoodinates = Coordinates;
        //zero direction flags
        memset(DirectionMatrix, 0, sizeof(DirectionMatrix));
        memset(ConnectivityZone, 0, sizeof(ConnectivityZone));
    }
    CellCoordinates getCellCoordinates() const
    {
        return thisCellCoodinates;
    }

    DirectionFlags DirectionMatrix[CUBESPERCELL];
    int ConnectivityZone[CUBESPERCELL];

    CellCoordinates thisCellCoodinates;
};

class KhazadGrid : public gridInterface
{
public:

    KhazadGrid(Map* TargetMap);

    void BuildConnectivityZones();

    DirectionFlags getDirectionFlags(const MapCoordinates &TargetCoords) const;

    int getConnectivityZone(const MapCoordinates &TargetCoords) const;
    void setConnectivityZone(const MapCoordinates &TargetCoords, int NewZone);

    void setDirectionFlags(const MapCoordinates& MapCoords, DirectionFlags Flags);

    std::map<uint32_t, int32_t>* getConnectivtySubMap(int Zone);

    void ChangeConnectivityMap(uint32_t FirstZone, uint32_t SecondZone, int32_t connectionChange);

    bool contains(const MapCoordinates &TestCoords) const;

    int max(Axis AxialComponent) const         { return maxlen[AxialComponent]; }
    int min(Axis AxialComponent) const         { return minlen[AxialComponent]; }

    GridCell* getCell(CellCoordinates TestCoords) const;
    GridCell* addCell(CellCoordinates TargetCoords);

    float getEdgeCost(const MapCoordinates &TestCoords, Direction DirectionType) const;

    bool isPathPossible(const MapCoordinates StartCoords, const MapCoordinates GoalCoords);

    uint32_t getZoneEquivilency(const MapCoordinates TargetCoords);

    void updateConnectivityCache(uint32_t Zone, uint32_t ZoneEquivilency);
    void RebuildConnectivityCache(uint32_t ZoneCount);

private:

    std::map<uint64_t, GridCell*> Cells;

    MapCoordinates maxlen;
    MapCoordinates minlen;

    std::vector<uint32_t> ConnectivityCache;
    std::map<uint32_t, std::map<uint32_t, int32_t> > ConnectivityMap;

};

#endif // GRID_HEADER
