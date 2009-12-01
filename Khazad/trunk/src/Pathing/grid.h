#ifndef GRID_HEADER
#define GRID_HEADER

#include <assert.h>
#include <vector>
#include <stdint.h>
#include <string.h>
#include <map>

#include <Cell.h>
#include <Coordinates.h>
#include <heuristics.h>

struct gridInterface  // Virtural Base class
{
    virtual ~gridInterface() {};

    virtual float getEdgeCost(const MapCoordinates &TestCoords, Direction DirectionType) const = 0;
    virtual int max(unsigned dim) const = 0;
    virtual int min(unsigned dim) const = 0;

    virtual bool contains(const MapCoordinates &TestCoords) const = 0;
};

typedef uint32_t DirectionFlags;  // Bitfield for all directions

struct GridCell
{
  GridCell() 
  {
    //zero direction flags
    memset(DirectionMatrix, 0, sizeof(DirectionMatrix));
  }
  DirectionFlags DirectionMatrix[CELLEDGESIZE][CELLEDGESIZE];
};

class KhazadGrid : public gridInterface
{
public:

    KhazadGrid()
    {
        std::map<uint64_t, Cell*>* TargetCells = MAP->getCellMap();
        for (std::map<uint64_t, Cell*>::iterator it = TargetCells->begin(); it != TargetCells->end(); ++it)
        {
            if (it->second != NULL)
            {
                Cell* TargetCell = it->second;
                CellCoordinates CellCoords = TargetCell->getCellCoordinates();
                GridCell* NewGridCell = addCell(CellCoords);
                CubeCoordinates TargetCubeCoords = CubeCoordinates(0, 0);
                for (int x = 0; x < CELLEDGESIZE; TargetCubeCoords.X++)
                {
                    for (int y = 0; y < CELLEDGESIZE; TargetCubeCoords.Y++)
                    {
                        int16_t TileShapeID = TargetCell->getCubeShape(TargetCubeCoords);

                        static int16_t NONE_ID = 0;
                        static int16_t FLOOR_ID = 1;
                        static int16_t WALL_ID = 2;
                        static int16_t RAMP_ID = 3;
                        static int16_t STAIR_ID = 4;
                        static int16_t FORT_ID = 5;
                        static int16_t EMPTY_ID = 6;
                        static int16_t BOULDER_ID = 7;

                        uint32_t Flags = 0;

                        if (TileShapeID == WALL_ID || TileShapeID == FORT_ID || TileShapeID == EMPTY_ID)
                        {
                            NewGridCell->DirectionMatrix[TargetCubeCoords.X][TargetCubeCoords.Y] = 0;
                            continue;
                        }

                        if (TileShapeID == RAMP_ID || TileShapeID == STAIR_ID)
                        {
                            for (Direction DirectionType = ANGULAR_DIRECTIONS_START; DirectionType < NUM_ANGULAR_DIRECTIONS; ++DirectionType)
                            {
                                int16_t AdjacentTileShape = MAP->getCubeShape(GenerateMapCoordinates(CellCoords, TargetCubeCoords));

                                if (DirectionType == DIRECTION_DOWN || DirectionType == DIRECTION_UP)
                                {
                                    if (AdjacentTileShape != TileShapeID && TileShapeID != STAIR_ID)
                                    {
                                        continue;  // Strait up and down is possible only through a ramp or stair
                                    }
                                }

                                if (AdjacentTileShape != WALL_ID && AdjacentTileShape != FORT_ID && AdjacentTileShape != EMPTY_ID)
                                {
                                    Flags |= (1 << (int) DirectionType);
                                }
                            }
                            NewGridCell->DirectionMatrix[TargetCubeCoords.X][TargetCubeCoords.Y] = Flags;
                            continue;
                        }

                        if (TileShapeID == FLOOR_ID || TileShapeID == BOULDER_ID)
                        {
                            for (Direction DirectionType = ANGULAR_DIRECTIONS_START; DirectionType < NUM_ANGULAR_DIRECTIONS; ++DirectionType)
                            {
                                int16_t AdjacentTileShape = MAP->getCubeShape(GenerateMapCoordinates(CellCoords, TargetCubeCoords));

                                if (DirectionType >= NUM_COMPASS_DIRECTIONS && isDirectionPositive(DirectionType))
                                {
                                    continue;  // Skip the upward directions, they are only valid for ramps/stairs
                                }

                                if (DirectionType == DIRECTION_DOWN || DirectionType == DIRECTION_UP)
                                {
                                    continue;  // Strait down is impossible through a floor
                                }

                                if (AdjacentTileShape == FLOOR_ID || AdjacentTileShape == RAMP_ID || AdjacentTileShape == STAIR_ID || AdjacentTileShape == BOULDER_ID)
                                {
                                    Flags |= (1 << (int) DirectionType);
                                }
                            }
                            NewGridCell->DirectionMatrix[TargetCubeCoords.X][TargetCubeCoords.Y] = Flags;
                        }
                    }
                }
                
            }
        }
    }

    MapCoordinates GenerateMapCoordinates(CellCoordinates CellCoords, CubeCoordinates CubeCoords)
    {
        MapCoordinates NewCoordinates;

        NewCoordinates.X = (CellCoords.X * CELLEDGESIZE) + CubeCoords.X;
        NewCoordinates.Y = (CellCoords.Y * CELLEDGESIZE) + CubeCoords.Y;
        NewCoordinates.Z = CellCoords.Z;

        return NewCoordinates;
    }

    DirectionFlags getDirectionFlags(const MapCoordinates &TargetCoords) const
    {
        GridCell* TargetCell = getCell(CellCoordinates(TargetCoords));
        if (TargetCell != NULL)
        {
            CubeCoordinates CubeCoords = CubeCoordinates(TargetCoords);
            return TargetCell->DirectionMatrix[CubeCoords.X][CubeCoords.Y];
        }
        return (DirectionFlags) 0;  // No connectivity because Cell is invalid
    }

    void setDirectionFlags(const MapCoordinates& MapCoords, DirectionFlags Flags)
    {
        GridCell* TargetCell = getCell(CellCoordinates(MapCoords));

        if (TargetCell == NULL)
        {
          TargetCell = new GridCell();
          uint64_t Key = GenerateCellKey(MapCoords);
          Cells[Key] = TargetCell;
        }
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

    bool contains(const MapCoordinates &TestCoords) const
    {
        return getCell(CellCoordinates(TestCoords)) != NULL;
    }
    
    int max(unsigned dim) const
    {
      return maxlen[dim];
    }
    
    int min(unsigned dim) const
    {
      return minlen[dim];
    }

    uint64_t GenerateCellKey(CellCoordinates KeyCoords) const
    {
        uint64_t Key = KeyCoords.X;
        Key <<= 16;
        Key += KeyCoords.Y;
        Key <<= 16;
        Key += KeyCoords.Z;

        return Key;
    }

    GridCell* getCell(CellCoordinates TestCoords) const
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

    GridCell* addCell(CellCoordinates TargetCoords)
    {
        GridCell* TargetCell = getCell(TargetCoords);
        if (TargetCell == NULL)
        {
            GridCell* NewGridCell = new GridCell();
            Cells[GenerateCellKey(TargetCoords)] = NewGridCell;
            return NewGridCell;
        }
        return TargetCell;
    }

    float getEdgeCost(const MapCoordinates &TestCoords, Direction DirectionType) const
    {
        count++;  // Count number of edges checked (for profiling path-finding algorithms)

        if (getDirectionFlags(TestCoords) & (1 << (int) DirectionType))
        {
            return 1;  // Standard cost, will need to be more complex in the future
        }
        return -1;  // No Edge exists
    }

    void zeroCount()                    { count = 0; }

    unsigned getCount() const           { return count; }

private:

    std::map<uint64_t, GridCell*> Cells;

    mutable unsigned count;
    MapCoordinates maxlen;
    MapCoordinates minlen;
};

#endif // GRID_HEADER
