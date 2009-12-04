#ifndef GRID_HEADER
#define GRID_HEADER

#include <assert.h>
#include <vector>
#include <stdint.h>
#include <string.h>
#include <map>

#include <Cell.h>
//#include <Coordinates.h>
#include <DataManager.h>
class MapCoordinates;
class Heuristic;

typedef uint32_t DirectionFlags;  // Bitfield for all directions

struct gridInterface  // Virtural Base class
{
    virtual ~gridInterface() {};

    virtual float getEdgeCost(const MapCoordinates &TestCoords, Direction DirectionType) const = 0;
    virtual int max(unsigned dim) const = 0;
    virtual int min(unsigned dim) const = 0;
    virtual DirectionFlags getDirectionFlags(const MapCoordinates &TargetCoords) const = 0;

    virtual bool contains(const MapCoordinates &TestCoords) const = 0;
};



struct GridCell
{
  GridCell(CellCoordinates Coordinates)
  {
    thisCellCoodinates = Coordinates;
    //zero direction flags
    memset(DirectionMatrix, 0, sizeof(DirectionMatrix));
  }
  CellCoordinates getCellCoordinates() const { return thisCellCoodinates; }
  
  DirectionFlags DirectionMatrix[CELLEDGESIZE][CELLEDGESIZE];
  CellCoordinates thisCellCoodinates;
};

class KhazadGrid : public gridInterface
{
public:

    KhazadGrid()
    {
        //static int16_t NONE_ID = 0;
        int16_t FLOOR_ID = DATA->getLabelIndex("TILESHAPE_FLOOR");
        //static int16_t WALL_ID = 2;
        int16_t RAMP_ID = DATA->getLabelIndex("TILESHAPE_RAMP");
        int16_t STAIR_ID = DATA->getLabelIndex("TILESHAPE_STAIR");
        //static int16_t FORT_ID = 5;
        //static int16_t EMPTY_ID = DATA->getLabelIndex("TILESHAPE_EMPTY");
        int16_t BOULDER_ID = DATA->getLabelIndex("TILESHAPE_BOULDER");
        int16_t STAIRDOWN_ID = DATA->getLabelIndex("TILESHAPE_DOWN_STAIR");
        std::map<uint64_t, Cell*>* TargetCells = MAP->getCellMap();
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

#define SHAPE_PASSABLE(shape) (((shape) == FLOOR_ID) || ((shape) == RAMP_ID) || ((shape) == STAIR_ID) || ((shape) == STAIRDOWN_ID) || ((shape) == BOULDER_ID))
                      
                        if (SHAPE_PASSABLE(TileShapeID))
                        {
                            Flags |= (1 << (int) DIRECTION_NONE);
                            for (Direction DirectionType = ANGULAR_DIRECTIONS_START; DirectionType < NUM_ANGULAR_DIRECTIONS; ++DirectionType)
                            {
                                assert((int) DIRECTION_DOWN == 0);
                                MapCoordinates AdjacentTileCoords = GenerateMapCoordinates(CellCoords, TargetCubeCoords);
                                AdjacentTileCoords.TranslateMapCoordinates(DirectionType);
                          
                                //see if we've done this already..
                                if (getDirectionFlags(AdjacentTileCoords) & (1 << (int) OppositeDirection(DirectionType)))
                                {
                                    Flags |= (1 << (int) DirectionType);
                                    continue;
                                }
                            
                                int16_t AdjacentTileShape = MAP->getCubeShape(AdjacentTileCoords);
                                
                                if (SHAPE_PASSABLE(AdjacentTileShape))
                                {
                                    if (DirectionType == DIRECTION_DOWN)
                                    {
                                        //only go down to stairs from down stairs
                                        if ((TileShapeID == STAIRDOWN_ID) && (AdjacentTileShape ==  STAIR_ID))
                                        {
                                            Flags |= (1 << (int) DirectionType);
                                        }
                                    } else if (DirectionType == DIRECTION_UP)
                                        {
                                        //only stairs go straight up
                                        if ((AdjacentTileShape == STAIRDOWN_ID) && (TileShapeID ==  STAIR_ID))
                                        {
                                            Flags |= (1 << (int) DirectionType);
                                        }
                                    } else if (DirectionType >= NUM_COMPASS_DIRECTIONS)
                                    {
                                        if (isDirectionPositive(DirectionType))
                                        {
                                            //only ramps go up-diagonal
                                            if (TileShapeID == RAMP_ID)
                                            {
                                                Flags |= (1 << (int) DirectionType);
                                            }
                                        } else
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
    }

    static MapCoordinates GenerateMapCoordinates(CellCoordinates CellCoords, CubeCoordinates CubeCoords)
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
            GridCell* NewGridCell = new GridCell(TargetCoords);
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
            if (DirectionType < CARDINAL_DIRECTIONS_START) // True for Up and Down
                return 2;
            if (DirectionType < NUM_CARDINAL_DIRECTIONS) // N, S, E, W
                return 1;
            if (DirectionType < NUM_COMPASS_DIRECTIONS) // NE, SE, NW, SW
                return M_SQRT2;
            if (DirectionType == DIRECTION_NONE)
                return 0;
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
