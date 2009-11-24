#ifndef PATH__HEADER
#define PATH__HEADER

#include <stdafx.h>
#include <Singleton.h>
#include <Map.h>

struct Path
{
    Path()
    {
        Length = -1;
    }

    float Length;

    // coordinate storage here
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

    Path FindPath(MapCoordinates StartCoords, MapCoordinates GoalCoords);
    float EstimatePathLength(MapCoordinates StartCoords, MapCoordinates GoalCoords);

protected:

};


#define PATH (PathManager::GetInstance())

#endif // PATH__HEADER
