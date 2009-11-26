#ifndef PATH_MANAGER__HEADER
#define PATH_MANAGER__HEADER

#include <stdafx.h>
#include <Singleton.h>
#include <MovementController.h>

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

    Path FindPath(int PathSystem, MapCoordinates StartCoords, MapCoordinates GoalCoords);
    float EstimatePathLength(int PathSystem, MapCoordinates StartCoords, MapCoordinates GoalCoords);

    int getExpandedNodeCount();
    int getGraphReads();
    bool isCacheHit();

    void ResetProfileData();

protected:

    // Path system storage here
};

#define PATH (PathManager::GetInstance())

#endif // PATH__HEADER
