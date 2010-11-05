#include <vector>
#include <algorithm>

#include <Path.h>


FullPath::FullPath(float PathLength, std::vector<MapCoordinates> Course)
{
    Length = PathLength;
    PathCourse = Course;
    StepCount = Course.size() - 1;

    StartCoordinates = PathCourse[0];
    GoalCoordinates = PathCourse[StepCount];
}

PathWalker* FullPath::getPathWalker()
{
    return new FullPathWalker(this);
}



FullPathWalker::FullPathWalker(FullPath* SourcePath)
{
    TargetPath = SourcePath;
    Reset();
}

void FullPathWalker::Reset()
{
    CurrentStep = 0;
}

MapCoordinates FullPathWalker::NextCoordinate()
{
    if(CurrentStep < TargetPath->StepCount)
    {
        CurrentStep++;
        return TargetPath->PathCourse[CurrentStep];
    }
    return TargetPath->GoalCoordinates; // Keep returning the Goal if we've reached the end of the path
}

Direction FullPathWalker::NextDirection()
{
    return DIRECTION_NONE;
}


VectorPath::VectorPath(float PathLength, std::vector<Direction> RawDirections, MapCoordinates StartCoords, MapCoordinates GoalCoords)
{
    StartCoordinates = StartCoords;
    GoalCoordinates = GoalCoords;
    StepCount = RawDirections.size();

    // Compress the raw directions into legs
    if (RawDirections.size() > 0)
    {
        int MagnitudeCounter = 1;
        Direction CurrentDirection = RawDirections[0];

        for (int i = 1; i < RawDirections.size(); i++)
        {
            if (RawDirections[i] != CurrentDirection)
            {
                Directions.push_back(CurrentDirection);
                Magnitudes.push_back(MagnitudeCounter);

                CurrentDirection = RawDirections[i];
                MagnitudeCounter = 1;
            }
            else
            {
                MagnitudeCounter++;
            }
        }
        Directions.push_back(CurrentDirection);
        Magnitudes.push_back(MagnitudeCounter);
    }
}

PathWalker* VectorPath::getPathWalker()
{
    return new VectorPathWalker(this);
}


VectorPathWalker::VectorPathWalker(VectorPath* SourcePath)
{
    TargetPath = SourcePath;
    Reset();
}

void VectorPathWalker::Reset()
{
    MagnitudeCountDown = TargetPath->Magnitudes[0];  // Prime the counter with the first legs magnitude
    LegCounter = 0;
    StepCoordinates = TargetPath->StartCoordinates;
    CurrentStep = 0;
}

MapCoordinates VectorPathWalker::NextCoordinate()
{
    if(CurrentStep < TargetPath->StepCount)
    {
        if (MagnitudeCountDown == 0)
        {
            LegCounter++;
            MagnitudeCountDown = TargetPath->Magnitudes[LegCounter];
        }

        CurrentStep++;
        MagnitudeCountDown--;

        StepCoordinates.TranslateMapCoordinates(TargetPath->Directions[LegCounter]);

        return StepCoordinates;
    }
    return TargetPath->GoalCoordinates; // Keep returning the Goal if we've reached the end of the path
}

Direction VectorPathWalker::NextDirection()
{
    if(CurrentStep < TargetPath->StepCount)
    {
        if (MagnitudeCountDown == 0)
        {
            LegCounter++;
            MagnitudeCountDown = TargetPath->Magnitudes[LegCounter];
        }

        CurrentStep++;
        MagnitudeCountDown--;

        return TargetPath->Directions[LegCounter];
    }
    return DIRECTION_NONE;
}
