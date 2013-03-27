#include <vector>
#include <algorithm>


#include <Grid.h>
#include <Game.h>
#include <Cell.h>
#include <Map.h>


KhazadGrid::KhazadGrid(Map* TargetMap)
{
    boost::unordered_map<uint64_t, Cell*>* TargetCells = TargetMap->getCellMap();
    for (boost::unordered_map<uint64_t, Cell*>::iterator it = TargetCells->begin(); it != TargetCells->end(); ++it)
    {
        if (it->second != NULL)
        {
            Cell* TargetCell = it->second;
            CellCoordinates CellCoords = TargetCell->getCellCoordinates();
            GridCell* NewGridCell = addCell(CellCoords);

            CubeCoordinates TargetCube = 0;
            do
            {
                CubeShape Shape = TargetCell->getCubeShape(TargetCube);
                uint32_t Flags = 0;

                if (!Shape.isSky() && !Shape.isSolid())
                {
                    MapCoordinates OverheadTileCoords = MapCoordinates(CellCoords, TargetCube);
                    OverheadTileCoords.TranslateMapCoordinates(DIRECTION_UP);
                    CubeShape OverheadCube = TargetMap->getCubeShape(OverheadTileCoords);
                    bool OverheadPassable = OverheadCube.isSky();

                    for (uint8_t i = 0; i < NUM_ANGULAR_DIRECTIONS; ++i)
                    {
                        MapCoordinates AdjacentTileCoords = MapCoordinates(CellCoords, TargetCube);
                        AdjacentTileCoords.TranslateMapCoordinates(Direction(Direction::ANGULAR_DIRECTIONS[i]));
                        Direction InvertedDirection = Direction(Direction::ANGULAR_DIRECTIONS[i]).Invert();

                        // if we've done this already..
                        if (getDirectionFlags(AdjacentTileCoords) & (1 << InvertedDirection.Index()))
                        {
                            Flags |= (1 << i);
                            continue;
                        }

                        CubeShape AdjacentCubeShape = TargetMap->getCubeShape(AdjacentTileCoords);

                        if (!AdjacentCubeShape.isSky() && !AdjacentCubeShape.isSolid())
                        {
                            if (Direction(Direction::ANGULAR_DIRECTIONS[i]).ValueonAxis(AXIS_Z) == 1)
                            {
                                if (OverheadPassable)
                                {
                                    Flags |= (1 << i);
                                }
                            } else {
                                //If no vertical direction, we only care that this tile is passable
                                Flags |= (1 << i);
                            }
                        }
                    }

                    NewGridCell->DirectionMatrix[TargetCube] = Flags;
                }
                TargetCube++;
            }
            while (TargetCube != 0);  // End Loop when Byte rolls over
        }
    }

    BuildConnectivityZones();
}

void KhazadGrid::BuildConnectivityZones()
{
    int ZoneCounter = 0;

    // Loop to do connectivity
    for (boost::unordered_map<uint64_t, GridCell*>::iterator it = Cells.begin(); it != Cells.end(); ++it)
    {
        GridCell* TargetCell = it->second;
        if (TargetCell != NULL)
        {
            CellCoordinates CellCoords = TargetCell->thisCellCoodinates;

            CubeCoordinates TargetCube = 0;
            do
            {
                uint32_t Flags = getDirectionFlags(MapCoordinates(CellCoords, TargetCube));

                if (Flags != 0)
                {
                    if (TargetCell->ConnectivityZone[TargetCube] == 0) // Start a new zone if not connected to another zone
                    {
                        ZoneCounter++; // First zone will be 1, 0 will indicate unititialized
                        TargetCell->ConnectivityZone[TargetCube] = ZoneCounter;
                    }
                     // Push this current zone onto the adjacent area
                    int CurrentZoneIndex = TargetCell->ConnectivityZone[TargetCube];

                    for (uint8_t i = 0; i < NUM_ANGULAR_DIRECTIONS; ++i)
                    {
                        if (Flags & (1 << i))
                        {
                            // Find the Zone that the adjcent Tile has
                            MapCoordinates AdjacentTileCoords = MapCoordinates(CellCoords, TargetCube);
                            AdjacentTileCoords.TranslateMapCoordinates(Direction::ANGULAR_DIRECTIONS[i]);
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
                    TargetCell->ConnectivityZone[TargetCube] = 0;
                }
                TargetCube++;
            }
            while (TargetCube != 0);  // End Loop when Byte rolls over
        }
    }

    RebuildConnectivityCache(ZoneCounter);
}

DirectionFlags KhazadGrid::getDirectionFlags(const MapCoordinates &TargetCoords) const
{
    GridCell* TargetCell = getCell(CellCoordinates(TargetCoords));
    if (TargetCell != NULL)
    {
        return TargetCell->DirectionMatrix[TargetCoords.Cube()];
    }
    return (DirectionFlags) 0;  // No connectivity because Cell is invalid
}

int KhazadGrid::getConnectivityZone(const MapCoordinates &TargetCoords) const
{
    GridCell* TargetCell = getCell(CellCoordinates(TargetCoords));
    if (TargetCell != NULL)
    {
        return TargetCell->ConnectivityZone[TargetCoords.Cube()];
    }
    return 0;  // No connectivity zone because Cell is invalid
}

void KhazadGrid::setConnectivityZone(const MapCoordinates &TargetCoords, int NewZone)
{
    GridCell* TargetCell = getCell(CellCoordinates(TargetCoords));
    if (TargetCell != NULL)
    {
        TargetCell->ConnectivityZone[TargetCoords.Cube()] = NewZone;
    }
}

void KhazadGrid::setDirectionFlags(const MapCoordinates& MapCoords, DirectionFlags Flags)
{
    //find the Target cell (add if necessary)
    GridCell* TargetCell = addCell(CellCoordinates(MapCoords));

    TargetCell->DirectionMatrix[MapCoords.Cube()] = Flags;

    for (Axis i = AXIS_START; i < NUM_AXIS; ++i)
    {
        if (MapCoords[i] > maxlen[i])
        {
            maxlen.Set(i, MapCoords[i]);
        }
        if (MapCoords[i] < minlen[i])
        {
            minlen.Set(i, MapCoords[i]);
        }
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

GridCell* KhazadGrid::getCell(CellCoordinates TestCoords) const
{
    uint64_t Key = TestCoords.Key();

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
        Cells[TargetCoords.Key()] = NewGridCell;
        return NewGridCell;
    }
    return TargetCell;
}

float KhazadGrid::getEdgeCost(const MapCoordinates &TestCoords, Direction DirectionType) const
{
	uint32_t Index = DirectionType.Index();
	uint32_t Flags = getDirectionFlags(TestCoords);

    if (Flags & (1 << Index))
    {
        if (DirectionType.ValueonAxis(AXIS_Z) != 0) // True for Up and Down
            return 2;

        bool X = DirectionType.ValueonAxis(AXIS_X) != 0;
        bool Y = DirectionType.ValueonAxis(AXIS_Y) != 0;

        if (X ^ Y) // N, S, E, W
            return 1;
        if (X & Y)
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

