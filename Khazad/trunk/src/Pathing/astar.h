#ifndef astar_HEADER
#define astar_HEADER

#include "point.h"
#include "heuristics.h"
#include "entry.h"
#include "zone.h"

struct PathAlgorithm
{
    virtual ~PathAlgorithm() {};

    virtual std::pair<cost_t,std::vector<point>> path (const point &s, const point &t) = 0;
    virtual void zeroCount() = 0;
    virtual unsigned getCount() const = 0;

};

struct AStar : PathAlgorithm
{
    unsigned count;

    AStar(const GridGraph *G, const Heuristic *h, const Heuristic *tb): G_(G), heuristic(h), tiebreaker(tb)
    {
        zeroCount();
    }

    void zeroCount()
    {
        count = 0;
    }

    unsigned getCount() const
    {
        return count;
    }

    std::pair<cost_t,std::vector<point>> path (const point &s, const point &t);

private:

    const GridGraph *G_;
    const Heuristic *heuristic, *tiebreaker;
};


struct ZonedAStar : PathAlgorithm
{
    unsigned count;
    unsigned cachemiss;

    ZonedAStar(const GridGraph *G, zoneManager *zm, const Heuristic *h, const Heuristic *tb) : G_(G), zm_(zm), heuristic(h), tiebreaker(tb)
    {
        zeroCount();
    }

    std::pair<cost_t,std::vector<point>> path (const point &s, const point &t);

    void zeroCount()
    {
        count = 0;
        cachemiss = 0;
    }

    unsigned getCount() const
    {
        return count;
    }

private:

    const Heuristic *heuristic, *tiebreaker;
    const GridGraph *G_;
    zoneManager *zm_;
};

#endif
