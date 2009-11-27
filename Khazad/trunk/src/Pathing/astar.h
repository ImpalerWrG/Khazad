#ifndef ASTAR_HEADER
#define ASTAR_HEADER

#include <Coordinates.h>
#include <heuristics.h>

#include <Path.h>
#include <Graph.h>
//#include "zone.h"

/*

class PathAlgorithm
{
public:

    virtual ~PathAlgorithm() {};

    virtual Path FindPath(const MapCoordinates &StartCoords, const MapCoordinates &GoalCoords) = 0;

    virtual void Reset() = 0;
    virtual unsigned getCount() const = 0;

private:

    unsigned count;
    const GridGraph* TargetSearchGraph;

    const Heuristic* MainHeuristic;
    const Heuristic* TieBreakerHeuristic;
};

class AStar : PathAlgorithm
{
public:

    AStar(const GridGraph *TargetSearchGraph, const Heuristic* MainHeuristicType, const Heuristic* TieBreakerHeuristicType)
    {
        SearchGraph = TargetSearchGraph;
        MainHeuristic = MainHeuristicType;
        TieBreakerHeuristic = TieBreakerHeuristicType;
        Reset();
    }

    void Reset()                    { count = 0; }
    unsigned getCount() const       { return count; }

    Path FindPath(const MapCoordinates &StartPoint, const MapCoordinates &GoalPoint);

private:

};

struct HierarchicalAStar : PathAlgorithm
{
public:

    HierarchicalAStar(const GridGraph* TargetSearchGraph, zoneManager *zm, const Heuristic* MainHeuristicType, const Heuristic* TieBreakerHeuristicType)
    {
        SearchGraph = TargetSearchGraph;
        MainHeuristic = MainHeuristicType;
        TieBreakerHeuristic = TieBreakerHeuristicType;
        Reset();
    }

    void Reset()                    { count = 0; cachemiss = 0;}
    unsigned getCount() const       { return count; }

    Path FindPath(const MapCoordinates &StartPoint, const MapCoordinates &GoalPoint);

private:

    int cachemiss;

    zoneManager* zm_;
};

*/

#endif // ASTAR_HEADER
