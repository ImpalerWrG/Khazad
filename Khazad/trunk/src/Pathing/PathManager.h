#ifndef PATH_MANAGER__HEADER
#define PATH_MANAGER__HEADER

#include <stdafx.h>
#include <Singleton.h>

#include <MovementController.h>

class MapCoordinates;
class KhazadGrid;
class AStar;
class Heuristic;

class PathManager
{
    DECLARE_SINGLETON_CLASS(PathManager)

public:

    ~PathManager();
    bool Init();

    void CreateMapAbstraction();
    void EditMapAbstraction();
    void DeleteMapAbstraction();

    MovementController getNewController(int AjentSize, int MovementType); // Create a new controler

    MapPath *FindPath(int PathSystem, MapCoordinates StartCoords, MapCoordinates GoalCoords);
    float EstimatePathLength(int PathSystem, MapCoordinates StartCoords, MapCoordinates GoalCoords);
    uint32_t getDirectionFlags(MapCoordinates Coordinates);
    bool contains(MapCoordinates Coordinates);

    int getExpandedNodeCount(int SystemIndex);
    int getGraphReads(int SystemIndex);
    bool isCacheHit(int SystemIndex);

    void ResetProfileData(int SystemIndex);

protected:

    AStar* AstarImplementation;
    //PathAlgorithm HeriarchialAstarImplementation;

    KhazadGrid* MapGrid;

    Heuristic* ManhattenHeuristic;
    Heuristic* EuclideanHeuristic;
    Heuristic* MaxDimensionHeuristic;
    Heuristic* DijkstraHeuristic;
    Heuristic* DiagonalHeuristic;
};

#define PATH (PathManager::GetInstance())

#endif // PATH__HEADER
