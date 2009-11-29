#ifndef ASTAR_HEADER
#define ASTAR_HEADER

#include <Coordinates.h>
#include <heuristics.h>

#include <Path.h>

class GridGraph;
class zoneManager;

class PathAlgorithm
{
public:

    virtual ~PathAlgorithm() {};

    virtual MapPath FindPath(const MapCoordinates &StartCoords, const MapCoordinates &GoalCoords) = 0;

    virtual void Reset() = 0;
    virtual unsigned getCount() const = 0;

protected:

    unsigned count;
    const GridGraph* SearchGraph;

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

    MapPath FindPath(const MapCoordinates &StartPoint, const MapCoordinates &GoalPoint) { return doFindPath(StartPoint, GoalPoint); }
    FullPath doFindPath(const MapCoordinates &StartPoint, const MapCoordinates &GoalPoint);

};

struct HierarchicalAStar : PathAlgorithm
{
public:

    HierarchicalAStar(const GridGraph* TargetSearchGraph, zoneManager *zm, const Heuristic* MainHeuristicType, const Heuristic* TieBreakerHeuristicType)
    {
        zm_ = zm;
        SearchGraph = TargetSearchGraph;
        MainHeuristic = MainHeuristicType;
        TieBreakerHeuristic = TieBreakerHeuristicType;
        Reset();
    }

    void Reset()                    { count = 0;}
    unsigned getCount() const       { return count; }

    MapPath FindPath(const MapCoordinates &StartPoint, const MapCoordinates &GoalPoint) { return doFindPath(StartPoint, GoalPoint); }

private:
    zoneManager* zm_;

    FullPath doFindPath(const MapCoordinates &StartPoint, const MapCoordinates &GoalPoint);
};

#endif // ASTAR_HEADER
