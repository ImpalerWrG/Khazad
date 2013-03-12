#include <vector>
#include <algorithm>

#include <MovementController.h>

#include <Grid.h>
#include <PathManager.h>

#include <Random.h>


MovementController::MovementController(int AjentSize, int MovementType, PathManager* Parent)
{
    CurrentPath = NULL;
    CurrentPathWalker = NULL;
    ParentManager = Parent;

    DirectionDice = new RandomNumberGenerator();
}

MovementController::~MovementController()
{
    if (CurrentPath != NULL)
    {
        delete CurrentPath;
    }

    if (CurrentPathWalker != NULL)
    {
        delete CurrentPathWalker;
    }
}

Direction MovementController::getNextStep()  // Next movement step for the ajent
{
    switch(CurrentMovementBehavior)
    {
        case PATH_BEHAVIOR_HALT:              // Agent stands still
        {
            return DIRECTION_NONE;
        }
        break;

        case PATH_BEHAVIOR_WANDER_DRUNKENLY:  // Agent wanders and ignores all obsticles and hazards
        {
            return (Direction) DirectionDice->Roll(0, NUM_ANGULAR_DIRECTIONS - 1);
        }
        break;

        case PATH_BEHAVIOR_WANDER_AIMLESSLY:  // Agent wanders but will not try to path through walls
        {
            uint32_t DirectionFlags = ParentManager->getDirectionFlags(CurrentLocation);

            std::vector<int> ValidDirections;
            int Position = 0;

            for (uint32_t TemporaryFlags = DirectionFlags; TemporaryFlags; TemporaryFlags >>= 1, Position++)
            {
                if (TemporaryFlags & 1)
                {
                    ValidDirections.push_back(Position);
                }
            }

            if (ValidDirections.size() > 0)
            {
                return (Direction) ValidDirections[DirectionDice->Roll(0, ValidDirections.size() - 1)];
            }

            return DIRECTION_NONE;
        }
        break;

        case PATH_BEHAVIOR_WANDER_IN_AREA:    // Agent wanders inside a confined area
        {
            return DIRECTION_NONE;
        }
        break;

        case PATH_BEHAVIOR_FOLLOW_ADJENT:     // Agent stay within a set distance of another ajent
        {
            return DIRECTION_NONE;
        }
        break;

        case PATH_BEHAVIOR_FLEE_ADJENT:       // Agent runs away from another adjent
        {
            return DIRECTION_NONE;
        }
        break;

        case PATH_BEHAVIOR_SEAK_ADJENT:       // Agent moves directly onto another ajent
        {
            return DIRECTION_NONE;
        }
        break;

        case PATH_BEHAVIOR_ROUTE_TO_LOCATION: // Agent follows a path to a specific location
        {
            return CurrentPathWalker->NextDirection();
        }
        break;

        case PATH_BEHAVIOR_ROUTE_TO_AREA:     // Agent follows a path to the closest location in an area
        {
            return DIRECTION_NONE;
        }
        break;

        case PATH_BEHAVIOR_TRAPPED:     // Agent is Trapped unable to reach a desired area
        {
            return DIRECTION_NONE;
        }
        break;
    }
}

void MovementController::LastStepInvalid()  // Tell the Controller the last step order was invalid
{

}

void MovementController::setBehaviorMode(MovementBehavior NewBehavior) // Set Crontroller to different behaviors such as wandering, halted, following, fleeing
{
    CurrentMovementBehavior = NewBehavior;

}

bool MovementController::ChangeDestination(MapCoordinates NewDestination)
{
    Destination = NewDestination;

    delete CurrentPath;
    CurrentPath = NULL;

    while (CurrentPath == NULL)
    {
        CurrentPath = ParentManager->FindPath(0, CurrentLocation, Destination);

        if (CurrentPath == NULL)
        {
            return false;
        }
    }

    if (CurrentPathWalker != NULL)
    {
        delete CurrentPathWalker;
    }
    CurrentPathWalker = CurrentPath->getPathWalker();

    return true;
}

void MovementController::setLocation(MapCoordinates NewLocation)
{
    CurrentLocation = NewLocation;
}

float MovementController::getPathEstimate(MapCoordinates TestDestination)
{
    return 0;
}

bool MovementController::isDestinationReachable(MapCoordinates TestDestination)
{
    return ParentManager->isPathPossible(0, CurrentLocation, TestDestination);
}
