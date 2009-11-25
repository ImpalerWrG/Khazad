#ifndef PATH__HEADER
#define PATH__HEADER

#include <stdafx.h>
#include <Singleton.h>
#include <Map.h>

struct Path
{
    Path()      { Length = -1; }

    float Length;
    int AdjentSize;
    int TerrainFlags;

    vector <MapCoordinates> PathCourse;
};

class PathManager
{
    DECLARE_SINGLETON_CLASS(PathManager)

public:

    ~PathManager();
    bool Init();

    void CreateMapAbstraction();
    void EditMapAbstraction();
    void DeleteMapAbstraction();

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
