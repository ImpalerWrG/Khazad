#ifndef ASTAR_HEADER
#define ASTAR_HEADER

#include <Path.h>

class gridInterface;
class zoneManager;
class MapCoordinates;
class Heuristic;

class PathAlgorithm
{
public:

    virtual ~PathAlgorithm() {};

    virtual MapPath *FindPath(const MapCoordinates &StartCoords, const MapCoordinates &GoalCoords) {};

    virtual void ResetPrifiler() {};

    virtual unsigned getGraphReads() const {};
    virtual unsigned getExpandedNodes() const {};

protected:

    unsigned GraphReads;
    unsigned ExpandedNodes;

    const gridInterface* SearchGraph;

    const Heuristic* MainHeuristic;
    const Heuristic* TieBreakerHeuristic;
};

class AStar : public PathAlgorithm
{
public:

    AStar(const gridInterface *TargetSearchGraph, const Heuristic* MainHeuristicType, const Heuristic* TieBreakerHeuristicType)
    {
        SearchGraph = TargetSearchGraph;
        MainHeuristic = MainHeuristicType;
        TieBreakerHeuristic = TieBreakerHeuristicType;
        ResetPrifiler();
    }

    void ResetPrifiler()                { GraphReads = 0; ExpandedNodes= 0; }

    unsigned getGraphReads() const      { return GraphReads; }
    unsigned getExpandedNodes() const    { return ExpandedNodes; }

    MapPath* FindPath(const MapCoordinates &StartPoint, const MapCoordinates &GoalPoint) { return doFindPath(StartPoint, GoalPoint); }
    FullPath* doFindPath(const MapCoordinates &StartPoint, const MapCoordinates &GoalPoint);

};

struct HierarchicalAStar : public PathAlgorithm
{
public:

    HierarchicalAStar(const gridInterface* TargetSearchGraph, zoneManager *zm, const Heuristic* MainHeuristicType, const Heuristic* TieBreakerHeuristicType)
    {
        zm_ = zm;
        SearchGraph = TargetSearchGraph;
        MainHeuristic = MainHeuristicType;
        TieBreakerHeuristic = TieBreakerHeuristicType;
        ResetPrifiler();
    }

    void ResetPrifiler()                { GraphReads = 0; ExpandedNodes= 0; }

    unsigned getGraphReads() const      { return GraphReads; }
    unsigned getExpandedNodes() const    { return ExpandedNodes; }

    MapPath *FindPath(const MapCoordinates &StartPoint, const MapCoordinates &GoalPoint) { return doFindPath(StartPoint, GoalPoint); }

private:
    zoneManager* zm_;

    FullPath *doFindPath(const MapCoordinates &StartPoint, const MapCoordinates &GoalPoint);
};

#endif // ASTAR_HEADER
