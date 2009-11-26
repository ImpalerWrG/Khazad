#ifndef GRID_HEADER
#define GRID_HEADER

#include <Coordinates.h>

struct gridInterface  // Virtural Base class
{
    virtual ~gridInterface() {};

    virtual float getEdgeCost( const MapCoordinates &StartCords, const MapCoordinates &GoalCoords) const = 0;

    virtual unsigned max(unsigned i) const = 0;
    virtual bool contains(const MapCoordinates &TestCoords) const = 0;
};

struct grid : gridInterface
{
    grid(point maxcorner): maxcorner(maxcorner), count(0)
    {
        tile = (char*)calloc(maxcorner[0] * maxcorner[1] * maxcorner[2], sizeof(char));
    }
    ~grid()
    {
        free(tile);
    }

    cost_t edgeCost(const point &pa, const point &pb) const
    {
        ++count;
        if (inBounds(pa) && inBounds(pb))
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

    void set(point p, char c)
    {
        tile[index(p)] = c;
    }

    unsigned max(unsigned i) const
    {
        return maxcorner[i];
    }

    bool contains(const point &p) const
    {
        return inBounds(p);
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

    bool inBounds(const point &p) const
    {
        for (unsigned i = 0; i < 3; ++i)
        {
            if (p[i] >= maxcorner[i])
            {
                return false;
            }
            if (p[i] < 0)
            {
                return false;
            }
        }
        return true;
    }
    unsigned index(const point &p) const
    {
        assert(inBounds(p));
        return p[0] + maxcorner[0] * p[1] + maxcorner[0] * maxcorner[1] * p[2];
    }

    point maxcorner;
    char *tile;
    mutable unsigned count;
};

#endif // GRID_HEADER
