#ifndef ASTAR_HEADER
#define ASTAR_HEADER

#include <boost/unordered_set.hpp>

class gridInterface;
class zoneManager;
class Heuristic;
class MapPath;
class FullPath;
class VectorPath;
class NodePool;

#include <Coordinates.h>

class PathAlgorithm
{
public:

    virtual ~PathAlgorithm() {};

    virtual void setEndPoints(const MapCoordinates &StartCoords, const MapCoordinates &GoalCoords)
    {
        StartCoordinates = StartCoords;
        GoalCoordinates = GoalCoords;
        GraphReads = ExpandedNodes = 0;
        FinalPath = NULL;
    };

    void setHeuristics(const Heuristic* PrimaryHeuristic, const Heuristic* SecondaryHeuristic)
    {
        MainHeuristic = PrimaryHeuristic;
        TieBreakerHeuristic = SecondaryHeuristic;
    };

    unsigned getGraphReads() const              { return GraphReads; };
    unsigned getExpandedNodes() const           { return ExpandedNodes; };

    virtual bool SearchPath(int NodesToExpand = 0) {};       // Search but do not return Path
    virtual MapPath* FindPath(int NodesToExpand = 0) {};     // Search and return the best Path

protected:

    unsigned GraphReads;
    unsigned ExpandedNodes;

    const gridInterface* SearchGraph;

    MapCoordinates StartCoordinates;
    MapCoordinates GoalCoordinates;

    const Heuristic* MainHeuristic;
    const Heuristic* TieBreakerHeuristic;

    MapPath* FinalPath;
};

class AStar : public PathAlgorithm
{
public:

    AStar(const gridInterface *TargetSearchGraph, Pool<AStarNode>* TargetPool);
    ~AStar();

    void setEndPoints(const MapCoordinates &StartCoords, const MapCoordinates &GoalCoords);

    bool SearchPath(int NodesToExpand = 0);
    MapPath* FindPath(int NodesToExpand = 0);

    inline bool ExpandNode();
    FullPath* GenerateFullPath();
    VectorPath* GenerateVectorPath();

protected:

    std::vector<AStarNode*> FringeNodes;
    boost::unordered_set<MapCoordinates, MapCoordinates::hash> VisitedCoordinates;
    AStarNode* CurrentNode;
    bool FringeExausted;
    Pool<AStarNode>* NodePool;
};

struct HierarchicalAStar : public PathAlgorithm
{
public:

    HierarchicalAStar(const gridInterface* TargetSearchGraph, zoneManager *zm)
    {
        zm_ = zm;
        SearchGraph = TargetSearchGraph;
        GraphReads = ExpandedNodes = 0;
    }

    MapPath* FindPath(const MapCoordinates &StartPoint, const MapCoordinates &GoalPoint) { return doFindPath(StartPoint, GoalPoint); }

private:
    zoneManager* zm_;

    MapPath* doFindPath(const MapCoordinates &StartPoint, const MapCoordinates &GoalPoint);
};

#endif // ASTAR_HEADER
