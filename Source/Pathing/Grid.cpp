#include <vector>
#include <algorithm>


#include "grid.h"
#include <Game.h>
#include <Cell.h>
#include <DataManager.h>


KhazadGrid::KhazadGrid(Map* TargetMap)
{
    int16_t FLOOR_ID = DATA->getLabelIndex("TILESHAPE_FLOOR");
    int16_t RAMP_ID = DATA->getLabelIndex("TILESHAPE_RAMP");
    int16_t STAIR_ID = DATA->getLabelIndex("TILESHAPE_STAIR");
    int16_t TREE_ID = DATA->getLabelIndex("TILESHAPE_TREE");
    int16_t BOULDER_ID = DATA->getLabelIndex("TILESHAPE_BOULDER");
    int16_t STAIRDOWN_ID = DATA->getLabelIndex("TILESHAPE_DOWN_STAIR");

    std::map<uint64_t, Cell*>* TargetCells = TargetMap->getCellMap();
    for (std::map<uint64_t, Cell*>::iterator it = TargetCells->begin(); it != TargetCells->end(); ++it)
    {
        if (it->second != NULL)
        {
            Cell* TargetCell = it->second;
            CellCoordinates CellCoords = TargetCell->getCellCoordinates();
            GridCell* NewGridCell = addCell(CellCoords);
            CubeCoordinates TargetCubeCoords = CubeCoordinates(0, 0);

            for (TargetCubeCoords.X = 0; TargetCubeCoords.X < CELLEDGESIZE; TargetCubeCoords.X++)
            {
                for (TargetCubeCoords.Y = 0; TargetCubeCoords.Y < CELLEDGESIZE; TargetCubeCoords.Y++)
                {
                    int16_t TileShapeID = TargetCell->getCubeShape(TargetCubeCoords);
                    uint32_t Flags = 0;

                    if (TileShapeID == FLOOR_ID || TileShapeID == RAMP_ID || TileShapeID == STAIR_ID || TileShapeID == STAIRDOWN_ID)
                    {
                        //Flags |= (1 << (int) DIRECTION_NONE);
                        for (Direction DirectionType = ANGULAR_DIRECTIONS_START; DirectionType < NUM_ANGULAR_DIRECTIONS; ++DirectionType)
                        {
                            MapCoordinates AdjacentTileCoords = MapCoordinates(CellCoords, TargetCubeCoords);
                            AdjacentTileCoords.TranslateMapCoordinates(DirectionType);

                            //see if we've done this already..
                            if (getDirectionFlags(AdjacentTileCoords) & (1 << (int) OppositeDirection(DirectionType)))
                            {
                                Flags |= (1 << (int) DirectionType);
                                continue;
                            }

                            int16_t AdjacentTileShape = TargetMap->getCubeShape(AdjacentTileCoords);

                            if (AdjacentTileShape == FLOOR_ID || AdjacentTileShape == RAMP_ID || AdjacentTileShape == STAIR_ID || AdjacentTileShape == STAIRDOWN_ID)
                            {
                                if (DirectionType == DIRECTION_DOWN)
                                {
                                    //only go down to stairs from down stairs
                                    if ((TileShapeID == STAIRDOWN_ID) && (AdjacentTileShape ==  STAIR_ID))
                                    {
                                        Flags |= (1 << (int) DirectionType);
                                    }
                                }
                                else if (DirectionType == DIRECTION_UP)
                                {
                                    //only stairs go straight up
                                    if ((AdjacentTileShape == STAIRDOWN_ID) && (TileShapeID ==  STAIR_ID))
                                    {
                                        Flags |= (1 << (int) DirectionType);
                                    }
                                }
                                else if (DirectionType >= NUM_COMPASS_DIRECTIONS)
                                {
                                    if (isDirectionPositive(DirectionType))
                                    {
                                        //only ramps go up-diagonal
                                        if (TileShapeID == RAMP_ID)
                                        {
                                            Flags |= (1 << (int) DirectionType);
                                        }
                                    }
                                    else
                                    {
                                        //only can go down-diagonal to ramp
                                        if (AdjacentTileShape == RAMP_ID)
                                        {
                                            Flags |= (1 << (int) DirectionType);
                                        }
                                    }
                                }
                                else
                                {
                                    //If no vertical direction, we only care that this tile is passable
                                    Flags |= (1 << (int) DirectionType);
                                }
                            }
                        }
                    }

                    NewGridCell->DirectionMatrix[TargetCubeCoords.X][TargetCubeCoords.Y] = Flags;
                }
            }
        }
    }

    BuildConnectivityZones();
}

void KhazadGrid::BuildConnectivityZones()
{
    int ZoneCounter = 0;
    CubeCoordinates TargetCubeCoords;

    // Loop to do connectivity
    for (std::map<uint64_t, GridCell*>::iterator it = Cells.begin(); it != Cells.end(); ++it)
    {
        if (it->second != NULL)
        {
            GridCell* TargetCell = it->second;
            CellCoordinates CellCoords = TargetCell->thisCellCoodinates;
            //ZoneCounter++;

            for (TargetCubeCoords.X = 0; TargetCubeCoords.X < CELLEDGESIZE; TargetCubeCoords.X++)
            {
                for (TargetCubeCoords.Y = 0; TargetCubeCoords.Y < CELLEDGESIZE; TargetCubeCoords.Y++)
                {
                    //uint32_t Flags = TargetCell->DirectionMatrix[TargetCubeCoords.X][TargetCubeCoords.Y];
                    uint32_t Flags = getDirectionFlags(MapCoordinates(CellCoords, TargetCubeCoords));

                    if (Flags != 0)
                    {
                        if (TargetCell->ConnectivityZone[TargetCubeCoords.X][TargetCubeCoords.Y] == 0) // Start a new zone if not connected to another zone
                        {
                            ZoneCounter++; // First zone will be 1, 0 will indicate unititialized
                            TargetCell->ConnectivityZone[TargetCubeCoords.X][TargetCubeCoords.Y] = ZoneCounter;
                        }
                         // Push this current zone onto the adjacent area
                        int CurrentZoneIndex = TargetCell->ConnectivityZone[TargetCubeCoords.X][TargetCubeCoords.Y];

                        for (Direction DirectionType = ANGULAR_DIRECTIONS_START; DirectionType < NUM_ANGULAR_DIRECTIONS; ++DirectionType)
                        {
                            if (Flags & (1 << (int) DirectionType))
                            {
                                // Find the Zone that the adjcent Tile has
                                MapCoordinates AdjacentTileCoords = MapCoordinates(CellCoords, TargetCubeCoords);
                                AdjacentTileCoords.TranslateMapCoordinates(DirectionType);
                                int AdjacentZoneIndex = getConnectivityZone(AdjacentTileCoords);

                                if (AdjacentZoneIndex == 0) // The other location is unititialized
                                {
                                    setConnectivityZone(AdjacentTileCoords, CurrentZoneIndex); // Push this current zone onto the adjacent area
                                }
                                else
                                {
                                    if (AdjacentZoneIndex != CurrentZoneIndex) // A different zone update the Connectivity Map
                                    {
                                        ChangeConnectivityMap(CurrentZoneIndex, AdjacentZoneIndex, 1);  // Add one connection between zones
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        TargetCell->ConnectivityZone[TargetCubeCoords.X][TargetCubeCoords.Y] = 0;
                    }
                }
            }
        }
    }

    RebuildConnectivityCache(ZoneCounter);
}

DirectionFlags KhazadGrid::getDirectionFlags(const MapCoordinates &TargetCoords) const
{
    GridCell* TargetCell = getCell(CellCoordinates(TargetCoords));
    if (TargetCell != NULL)
    {
        CubeCoordinates CubeCoords = CubeCoordinates(TargetCoords);
        return TargetCell->DirectionMatrix[CubeCoords.X][CubeCoords.Y];
    }
    return (DirectionFlags) 0;  // No connectivity because Cell is invalid
}

int KhazadGrid::getConnectivityZone(const MapCoordinates &TargetCoords) const
{
    GridCell* TargetCell = getCell(CellCoordinates(TargetCoords));
    if (TargetCell != NULL)
    {
        CubeCoordinates CubeCoords = CubeCoordinates(TargetCoords);
        return TargetCell->ConnectivityZone[CubeCoords.X][CubeCoords.Y];
    }
    return 0;  // No connectivity zone because Cell is invalid
}

void KhazadGrid::setConnectivityZone(const MapCoordinates &TargetCoords, int NewZone)
{
    GridCell* TargetCell = getCell(CellCoordinates(TargetCoords));
    if (TargetCell != NULL)
    {
        CubeCoordinates CubeCoords = CubeCoordinates(TargetCoords);
        TargetCell->ConnectivityZone[CubeCoords.X][CubeCoords.Y] = NewZone;
    }
}

void KhazadGrid::setDirectionFlags(const MapCoordinates& MapCoords, DirectionFlags Flags)
{
    //find the Target cell (add if necessary)
    GridCell* TargetCell = addCell(CellCoordinates(MapCoords));

    CubeCoordinates CubeCoords = CubeCoordinates(MapCoords);
    TargetCell->DirectionMatrix[CubeCoords.X][CubeCoords.Y] = Flags;
    for (int i = 0; i < 3; i++)
    {
        if (MapCoords[i] > maxlen[i])
            maxlen.set(i,MapCoords[i]);
        if (MapCoords[i] < minlen[i])
            minlen.set(i,MapCoords[i]);
    }
}

std::map<uint32_t, int32_t>* KhazadGrid::getConnectivtySubMap(int Zone)
{
    std::map<uint32_t, std::map<uint32_t, int32_t> >::iterator it;
    it = ConnectivityMap.find(Zone);

    if (it == ConnectivityMap.end())
    {
        std::map<uint32_t, int32_t>* TargetMap = new std::map<uint32_t, int32_t>;
        ConnectivityMap[Zone] = *TargetMap;
        return TargetMap;
    }
    else
    {
        return &it->second;
    }
}

void KhazadGrid::ChangeConnectivityMap(uint32_t FirstZone, uint32_t SecondZone, int32_t connectionChange)
{
    if (FirstZone == SecondZone)
    {
        return;  // Cannot connect to self
    }

    std::map<uint32_t, int32_t>* TargetMap = getConnectivtySubMap(FirstZone);
    std::map<uint32_t, int32_t>::iterator it;
    it = TargetMap->find(SecondZone);

    if (it == TargetMap->end())
    {
        if (connectionChange > 0)  // Do not allow negative connection counts
        {
            TargetMap->insert(pair<uint32_t, int32_t>(SecondZone, connectionChange));
        }
    }
    else
    {
        it->second += connectionChange;

        if (it->second <= 0)
        {
            TargetMap->erase(it); // Remove the connection entirely
        }
    }
}

bool KhazadGrid::contains(const MapCoordinates &TestCoords) const
{
    return getCell(CellCoordinates(TestCoords)) != NULL;
}

uint64_t KhazadGrid::GenerateCellKey(CellCoordinates KeyCoords) const
{
    uint64_t Key = KeyCoords.X;
    Key <<= 16;
    Key += KeyCoords.Y;
    Key <<= 16;
    Key += KeyCoords.Z;

    return Key;
}

GridCell* KhazadGrid::getCell(CellCoordinates TestCoords) const
{
    uint64_t Key = GenerateCellKey(TestCoords);

    if (Cells.find(Key) == Cells.end())
    {
        return NULL;
    }
    else
    {
        return Cells.find(Key)->second;
    }
}

GridCell* KhazadGrid::addCell(CellCoordinates TargetCoords)
{
    GridCell* TargetCell = getCell(TargetCoords);
    if (TargetCell == NULL)
    {
        GridCell* NewGridCell = new GridCell(TargetCoords);
        Cells[GenerateCellKey(TargetCoords)] = NewGridCell;
        return NewGridCell;
    }
    return TargetCell;
}

float KhazadGrid::getEdgeCost(const MapCoordinates &TestCoords, Direction DirectionType) const
{
    if (getDirectionFlags(TestCoords) & (1 << (int) DirectionType))
    {
        if (DirectionType < CARDINAL_DIRECTIONS_START) // True for Up and Down
            return 2;
        if (DirectionType < NUM_CARDINAL_DIRECTIONS) // N, S, E, W
            return 1;
        if (DirectionType < NUM_COMPASS_DIRECTIONS) // NE, SE, NW, SW
            return M_SQRT2;
        if (DirectionType == DIRECTION_NONE)
            return 0;
        return 2;  // All other z axis diagonals
    }
    return -1;  // No Edge exists
}

bool KhazadGrid::isPathPossible(const MapCoordinates StartCoords, const MapCoordinates GoalCoords)
{
    return getZoneEquivilency(StartCoords) == getZoneEquivilency(GoalCoords);
}

uint32_t KhazadGrid::getZoneEquivilency(const MapCoordinates TargetCoords)
{
    return ConnectivityCache[getConnectivityZone(TargetCoords)];
}

void KhazadGrid::updateConnectivityCache(uint32_t Zone, uint32_t ZoneEquivilency)
{
    ConnectivityCache[Zone] = ZoneEquivilency;

    std::map<uint32_t, int32_t>* TargetMap = getConnectivtySubMap(Zone);
    for (std::map<uint32_t, int32_t>::iterator it = TargetMap->begin(); it != TargetMap->end(); ++it)
    {
        if (ConnectivityCache[it->first] == 0) // || it->first > ZoneEquivilency)
        {
            updateConnectivityCache(it->first, ZoneEquivilency);  // Recursivly push the lowest zone to all children
        }
    }
}

void KhazadGrid::RebuildConnectivityCache(uint32_t ZoneCount)
{
    ConnectivityCache.reserve(ZoneCount);
    ConnectivityCache.assign(ZoneCount, 0);

    for (uint32_t i = 1; i < ConnectivityCache.size(); i++)  // Skip zero, no such zone
    {
        if (ConnectivityCache[i] == 0)
        {
            updateConnectivityCache(i, i);
        }
    }
}

