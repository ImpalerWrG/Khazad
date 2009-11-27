#ifndef PATH__HEADER
#define PATH__HEADER

#include <Coordinates.h>
#include <vector.h>

using namespace std;

struct MapPath
{
    float Length;   // The travel cost of the path
    int StepCount;  // The number of individual steps in the path

    int SizeLimit;      // Largest size of adjent that can use this path
    int MovementFlags;  // Booleans flags for terrain passable flags

    MapCoordinates StartCoordinates, GoalCoordinates;
};

struct FullPath: MapPath
{
    vector<MapCoordinates> PathCourse;
};

struct VectorPath: MapPath
{
    //vector<Direction> Directions;  // Needs a definition of Directions
    vector<int> Magnitudes;
};

struct WayPointPath: MapPath
{
    vector<MapCoordinates> WayPoints;
};

#endif // PATH__HEADER
