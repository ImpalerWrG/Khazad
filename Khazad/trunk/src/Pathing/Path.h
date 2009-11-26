#ifndef PATH__HEADER
#define PATH__HEADER

#include <Coordinates.h>
#include <vector.h>

using namespace std;

struct Path
{
    float Length;   // The travel cost of the path
    int StepCount;  // The number of individual steps in the path

    int SizeLimit;      // Largest size of adjent that can use this path
    int MovementFlags;  // Booleans flags for terrain passable flags

    MapCoordinates StartCoordinates, GoalCoordinates;
};

struct FullPath: Path
{
    vector<MapCoordinates> PathCourse;
};

struct VectorPath: Path
{
    //vector<Direction> Directions;  // Needs a definition of Directions
    vector<int> Magnitudes;
};

struct WayPointPath: Path
{
    vector<MapCoordinates> WayPoints;
};

#endif // PATH__HEADER
