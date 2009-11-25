#ifndef ASTAR_HEADER
#define ASTAR_HEADER

#include "point.h"
#include "heuristics.h"
#include "entry.h"
#include "zone.h"

struct PathAlgorithm
{
    virtual ~PathAlgorithm() {};

    virtual Path FindPath(const point &StartPoint, const point &GoalPoint) = 0;

    virtual void Reset() = 0;
    virtual unsigned getCount() const = 0;

};

class AStar : PathAlgorithm
{
public:

    AStar(const GridGraph *TargetSearchGraph, const Heuristic* MainHeuristicType, const Heuristic* TieBreakerHeuristicType)
    {
        SearchGraph = TargetSearchGraph
        MainHeuristic = MainHeuristicType;
        TieBreakerHeuristic = TieBreakerHeuristicType;
        zeroCount();
    }

    void Reset()                    { count = 0; }
    unsigned getCount() const       { return count; }

    Path FindPath(const point &StartPoint, const point &GoalPoint);

private:

    unsigned count;
    const GridGraph* TargetSearchGraph;

    const Heuristic* MainHeuristic;
    const Heuristic* TieBreakerHeuristic;
};

struct ZonedAStar : PathAlgorithm
{
public:

    ZonedAStar(const GridGraph* TargetSearchGraph, zoneManager *zm, const Heuristic* MainHeuristicType, const Heuristic* TieBreakerHeuristicType)
    {
        SearchGraph = TargetSearchGraph;
        MainHeuristic = MainHeuristicType;
        TieBreakerHeuristic = TieBreakerHeuristicType;
        zeroCount();
    }

    Path FindPath(const point &StartPoint, const point &GoalPoint);

    void Reset()                    { count = 0; cachemiss = 0;}
    unsigned getCount() const       { return count; }

private:

    unsigned count;
    unsigned cachemiss;

    const GridGraph* SearchGraph;

    const Heuristic* MainHeuristic;
    const Heuristic* TieBreakerHeuristic;

    zoneManager* zm_;
};

#endif // ASTAR_HEADER
