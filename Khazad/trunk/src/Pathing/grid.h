#ifndef GRID_HEADER
#define GRID_HEADER

#include <assert.h>
#include <vector>
#include <stdint.h>
#include <string.h>
#include <map>

#include <Coordinates.h>
#include <heuristics.h>

struct gridInterface  // Virtural Base class
{
    virtual ~gridInterface() {};

    virtual float getEdgeCost(const MapCoordinates &TestCoords, Direction DirectionType) const = 0;
    virtual unsigned max(int dim) const = 0;

    virtual bool contains(const MapCoordinates &TestCoords) const = 0;
};

typedef uint32_t DirectionFlags;  // Bitfield for all directions

struct GridCell
{
  GridCell() 
  {
    //zero direction flags
    memset(DirectionMatrix,0,sizeof(DirectionMatrix));
  }
  DirectionFlags DirectionMatrix[CELLEDGESIZE][CELLEDGESIZE];
};

class KhazadGrid : public gridInterface
{
public:

    KhazadGrid()
    {
        /*for (int x = 0; x < 0; x++)
        {
            //DirectionFlags Flags = 0;
            //MapCoordinates TestCoords = MapCoordinates(x,y,z);

            for (Direction DirectionType = ANGULAR_DIRECTIONS_START; DirectionType < NUM_ANGULAR_DIRECTIONS; ++DirectionType)
            {
                
                if (getEdgeCost(TestCoords, DirectionType) >= 0)
                {
                    Flags |= (1 << (int) DirectionType);
                }
                
            }
            //setDirectionFlags(TestCoords, Flags);
        }*/
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
           if (MapCoords[i] > maxlen[i])
             maxlen.set(i,MapCoords[i]);
    }

    bool contains(const MapCoordinates &TestCoords) const
    {
        return getCell(CellCoordinates(TestCoords)) != NULL;
    }
    
    unsigned max(int dim) const
    {
      return maxlen[dim];
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
};

#endif // GRID_HEADER
