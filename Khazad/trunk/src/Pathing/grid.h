#ifndef GRID_HEADER
#define GRID_HEADER

#include <assert.h>
#include <vector>
//#include <stdio.h>

#include <Coordinates.h>
#include "heuristics.h"

struct gridInterface  // Virtural Base class
{
    virtual ~gridInterface() {};

    virtual cost_t getEdgeCost( const MapCoordinates &StartCords, const MapCoordinates &GoalCoords) const = 0;
    cost_t edgeCost( const MapCoordinates &StartCords, const MapCoordinates &GoalCoords) const { getEdgeCost(StartCords, GoalCoords); } 

    virtual unsigned max(unsigned i) const = 0;
    bool contains(const MapCoordinates &TestCoords) const 
    {
      for (unsigned i = 0; i < 3; ++i)
      {
          if (TestCoords[i] >= max(i))
          {
              return false;
          }
          if (TestCoords[i] < 0)
          {
              return false;
          }
      }
      return true;
    }
};



struct grid : gridInterface
{
  public:
    grid(MapCoordinates maxcorner) : maxcorner(maxcorner), count(0)
    {
        count = 0;
        tile = (char*)calloc(maxcorner[0] * maxcorner[1] * maxcorner[2], sizeof(char));
    }
    ~grid()
    {
        free(tile);
    }

    cost_t getEdgeCost(const MapCoordinates &pa, const MapCoordinates &pb) const
    {
        ++count;
        if (contains(pa) && contains(pb))
        {
            char ca, cb;
            ca = tile[index(pa)];
            cb = tile[index(pb)];

            if ((ca == '#') || (cb == '#'))  //no walking through walls
                return -1;

            int dx,dy,dz;
            dx = pa[0]-pb[0];
            dy = pa[1]-pb[1];
            dz = pa[2]-pb[2];
            if (dz != 0)
            {
                //going up
                if ((dx != 0) || (dy != 0))
                {
                    //diagonally up only for ramps
                    if (((dz < 0) && (cb == 'r')) || ((dz > 0) && (ca == 'r')))
                        //ramp
                        return 1;
                    else
                        return -1;
                }
                else
                {
                    //straight up, only for stairs (requires both up and down (or up/down))
                    if (((dz < 0) && ((cb == 'u') || (cb == 'i')) && ((ca == 'j') || (ca == 'i'))) ||
                            ((dz > 0) && ((ca == 'u') || (ca == 'i')) && ((cb == 'j') || (cb == 'i'))))
                        //stairs!
                        return 1;
                    else
                        return -1;
                }
            }
            else
            {
                //all other horizontal allowed
                return 1;
            }
        }
        else
            return -1;
    }

    void set(MapCoordinates p, char c)
    {
        tile[index(p)] = c;
    }

    unsigned max(unsigned i) const
    {
        return maxcorner[i];
    }

    void zeroCount()
    {
        count = 0;
    }
    unsigned getCount() const
    {
        return count;
    }

  private:

     unsigned index(const MapCoordinates &p) const
    {
        assert(contains(p));
        return p[0] + maxcorner[0] * p[1] + maxcorner[0] * maxcorner[1] * p[2];
    }

    MapCoordinates maxcorner;
    char *tile;
    mutable unsigned count;
};

typedef enum
{
   None = 0,
   West,
   NorthWest,
   North,
   NorthEast,
   East,
   SouthEast,
   South,
   SouthWest,
   Up,
   UpWest,
   UpNorthWest,
   UpNorth,
   UpNorthEast,
   UpEast,
   UpSouthEast,
   UpSouth,
   UpSouthWest,
   Down,
   DownWest,
   DownNorthWest,
   DownNorth,
   DownNorthEast,
   DownEast,
   DownSouthEast,
   DownSouth,
   DownSouthWest
} Direction;

static uint32_t pointsToDirection(const MapCoordinates &pa, const MapCoordinates &pb)
{
  uint32_t direction = 0;
  //determine the direction
  if (pa[2] > pb[2])
    direction = Direction::Up;
  else if (pa[2] < pb[2])
    direction = Direction::Down;
  if (pa[1] > pb[1])
  {
    if (pa[0] > pb[0])
      direction = direction + Direction::NorthWest;
    else if (pa[0] < pb[0])
      direction = direction + Direction::NorthEast;
    else
      direction = direction + Direction::North;
  }
  else if (pa[1] < pb[1])
  {
    if (pa[0] > pb[0])
      direction = direction + Direction::SouthWest;
    else if (pa[0] < pb[0])
      direction = direction + Direction::SouthEast;
    else
      direction = direction + Direction::South;
  }
  else
  {
    if (pa[0] > pb[0])
      direction = direction + Direction::West;
    else if (pa[0] < pb[0])
      direction = direction + Direction::East;
  }
  return direction;
}

struct ImpalerCanRenameThisGrid : gridInterface
{
  unsigned len[3];
  std::vector<std::vector<std::vector<uint32_t>>> database;
  
   
  ImpalerCanRenameThisGrid()
  {
    for (int i = 0; i < 3; i++)
      len[i] = 0;
  }
  
  ImpalerCanRenameThisGrid(gridInterface *G)
  {
    for (int i = 0; i < 3; i++)
      len[i] = 0;
    for (int x = 0; x < G->max(0); x++)
    {
      for (int y = 0; y < G->max(1); y++)
      {
        for (int z = 0; z < G->max(2); z++)
        {
          uint32_t direction = 0;
          MapCoordinates p(x,y,z);
          
          //for each possible direction
          for (int idx = 0; idx < 27; idx++)
          {
            MapCoordinates pb;
            int offset = idx;
            for (unsigned j = 0; j < 3; ++j) {
              switch (offset % 3) {
                case 0: pb.set(j, p[j]); break;
                case 1: pb.set(j, p[j] + 1); break;
                case 2: pb.set(j, p[j] - 1); break;
              }
              offset /= 3;
            }
            if (G->getEdgeCost(p,pb) >= 0)
              direction |= (1 << pointsToDirection(p,pb));
          }
          setTileAllowedDirections(p,direction);
          //printf("(%d,%d,%d) %X\n", p[0], p[1], p[2], direction);
        }
      }
    }
    for (int x = 0; x < G->max(0); x++)
    {
      for (int y = 0; y < G->max(1); y++)
      {
        for (int z = 0; z < G->max(2); z++)
        {
          MapCoordinates p(x,y,z);
          for (int idx = 0; idx < 27; idx++)
          {
            MapCoordinates pb;
            int offset = idx;
            for (unsigned j = 0; j < 3; ++j) {
              switch (offset % 3) {
                case 0: pb.set(j, p[j]); break;
                case 1: pb.set(j, p[j] + 1); break;
                case 2: pb.set(j, p[j] - 1); break;
              }
              offset /= 3;
            }
            assert(G->getEdgeCost(p,pb) == getEdgeCost(p,pb));
          }
        }
      }
    }
  }

  unsigned max(unsigned i) const 
  {
     return len[i];
  }
  
  cost_t getEdgeCost(const MapCoordinates &pa, const MapCoordinates &pb) const
  {
    count++;
    if (contains(pa) && contains(pb))
      return ((database[pa[0]][pa[1]][pa[2]] & (1 << pointsToDirection(pa,pb))) ? 1 : -1);
    else return -1;
  }
  
  void setTileAllowedDirections(const MapCoordinates& p, uint32_t directions)
  {
    if (p[0] >= len[0])
    {
      for (int x = len[0]; x <= p[0]; x++)
      {
        database.push_back(std::vector<std::vector<uint32_t>>(len[1],std::vector<uint32_t>(len[2],0)));
      }
      len[0] = p[0]+1;
    }
    if (p[1] >= len[1])
    {
      for (int x = 0; x < len[0]; x++)
        for (int y = len[1]; y <= p[1]; y++)
          database[x].push_back(std::vector<uint32_t>(len[2],0));
      len[1] = p[1]+1;
    }
    if (p[2] >= len[2])
    {
      for (int x = 0; x < len[0]; x++)
        for (int y = 0; y < len[1]; y++)
          for (int z = len[2]; z <= p[2]; z++)
            database[x][y].push_back(0);
      len[2] = p[2]+1;
    }

    database[p[0]][p[1]][p[2]] = directions;
  }
  
  void zeroCount()
  {
    count = 0;
  }
  
  unsigned getCount() const
  {
    return count;
  }
    
  mutable unsigned count;
};


#endif // GRID_HEADER
