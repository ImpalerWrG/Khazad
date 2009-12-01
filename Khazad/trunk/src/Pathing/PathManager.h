#ifndef PATH_MANAGER__HEADER
#define PATH_MANAGER__HEADER

#include <stdafx.h>
#include <Singleton.h>
#include <MovementController.h>
#include <astar.h>
#include <grid.h>

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

    int getExpandedNodeCount()      { return 0; }
    int getGraphReads()             { return 0; }
    bool isCacheHit()               { return false; }

    void ResetProfileData()         { };

protected:

    AStar* AstarImplementation;
    //PathAlgorithm HeriarchialAstarImplementation;

    KhazadGrid* MapGrid;
};

#define PATH (PathManager::GetInstance())

#endif // PATH__HEADER
