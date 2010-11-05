#ifndef PATH__HEADER
#define PATH__HEADER

#include <Coordinates.h>
#include <vector>

class PathWalker
{
public:

    virtual MapCoordinates NextCoordinate() = 0;
    virtual Direction NextDirection() = 0;
    virtual void Reset() = 0;

    int CurrentStep;
};

class MapPath
{
public:

    float Length;   // The travel cost of the path
    int StepCount;  // The number of individual steps in the path

    int SizeLimit;      // Largest size of adjent that can use this path
    int MovementFlags;  // Booleans flags for terrain passable flags

    virtual PathWalker* getPathWalker() = 0;

    MapCoordinates StartCoordinates, GoalCoordinates;
};

class FullPath: public MapPath
{
public:

    FullPath(float PathLength, std::vector<MapCoordinates> Course);
    PathWalker* getPathWalker();

    std::vector<MapCoordinates> PathCourse;
};

class FullPathWalker: public PathWalker
{
public:

    FullPathWalker(FullPath* SourcePath);

    MapCoordinates NextCoordinate();
    Direction NextDirection();
    void Reset();

    FullPath* TargetPath;
};

class VectorPath: public MapPath
{
public:

    VectorPath(float PathLength, std::vector<Direction> RawDirections, MapCoordinates StartCoords, MapCoordinates GoalCoords);
    PathWalker* getPathWalker();

    std::vector<Direction> Directions;
    std::vector<int> Magnitudes;
};

class VectorPathWalker: public PathWalker
{
public:

    VectorPathWalker(VectorPath* SourcePath);

    MapCoordinates NextCoordinate();
    Direction NextDirection();
    void Reset();

private:

    MapCoordinates StepCoordinates;
    int MagnitudeCountDown;
    int LegCounter;

    VectorPath* TargetPath;
};

/*
class WayPointPath: public MapPath
{
    std::vector<MapCoordinates> WayPoints;
};
*/

#endif // PATH__HEADER
