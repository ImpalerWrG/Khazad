#ifndef PATH__HEADER
#define PATH__HEADER

#include "Coordinates.h"
#include <vector>

struct MapPath
{
    MapPath() {};

    virtual MapCoordinates NextCoordinate()  {};
    //virtual Direction NextDirection();

    virtual void ResetSteps() {}; // Reset to the start of the path

    float Length;   // The travel cost of the path
    int StepCount;  // The number of individual steps in the path
    int CurrentStep;  // Used to iterate the path

    int SizeLimit;      // Largest size of adjent that can use this path
    int MovementFlags;  // Booleans flags for terrain passable flags

    MapCoordinates StartCoordinates, GoalCoordinates;
};

struct FullPath: MapPath
{
    FullPath ()
    {
        Length = -1;
        StepCount = 0;
        CurrentStep = 0;
    }

    FullPath (float PathLength, std::vector<MapCoordinates> Course)
    {
        Length = PathLength;
        PathCourse = Course;
        StepCount = Course.size() - 1;
        CurrentStep = 0;

        StartCoordinates = PathCourse[0];
        GoalCoordinates = PathCourse[StepCount];
    }

    void ResetSteps()
    {
        CurrentStep = 0;
    }

    MapCoordinates NextCoordinate()
    {
        if(CurrentStep < StepCount)
        {
            CurrentStep++;
            return PathCourse[CurrentStep];
        }
        return GoalCoordinates; // Keep returning the Goal if we've reached the end of the path
    }

    std::vector<MapCoordinates> PathCourse;
};

struct VectorPath: MapPath
{
    VectorPath()
    {

    }

    void ResetSteps()
    {
        CurrentStep = 0;
        MagnitudeCountDown = Magnitudes[0];  // Prime the counter with the first legs magnitude
        LegCounter = 0;
        StepCoordinates = StartCoordinates;
    }

    MapCoordinates NextCoordinate()
    {
        if(CurrentStep < StepCount)
        {
            if (MagnitudeCountDown == 0)
            {
                LegCounter++;
                MagnitudeCountDown = Magnitudes[LegCounter];
            }

            CurrentStep++;
            MagnitudeCountDown--;

            StepCoordinates.TranslateMapCoordinates(Directions[LegCounter]);

            return StepCoordinates;
        }
        return GoalCoordinates; // Keep returning the Goal if we've reached the end of the path
    }

    Direction NextDirection()
    {
        if(CurrentStep < StepCount)
        {
            if (MagnitudeCountDown == 0)
            {
                LegCounter++;
                MagnitudeCountDown = Magnitudes[LegCounter];
            }

            CurrentStep++;
            MagnitudeCountDown--;

            return Directions[LegCounter];
        }
        return DIRECTION_NONE;
    }

    std::vector<Direction> Directions;
    std::vector<int> Magnitudes;

private:

    MapCoordinates StepCoordinates;
    int MagnitudeCountDown;
    int LegCounter;
};

struct WayPointPath: MapPath
{
    MapCoordinates NextCoordinate()
    {
        return GoalCoordinates;
    }

    void ResetSteps()
    {
        CurrentStep = 0;
    }

    std::vector<MapCoordinates> WayPoints;
};

#endif // PATH__HEADER
