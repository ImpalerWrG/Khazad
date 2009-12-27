#include <vector>
#include <algorithm>

#include <MovementController.h>

#include <grid.h>
#include <Random.h>
#include <PathManager.h>



MovementController::MovementController(int AjentSize, int MovementType)
{

}

MovementController::~MovementController()
{

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
            return (Direction) RANDOM->Roll(0, NUM_ANGULAR_DIRECTIONS - 1);
        }
        break;

        case PATH_BEHAVIOR_WANDER_AIMLESSLY:  // Agent wanders but will not try to path through walls
        {
            uint32_t DirectionFlags = PATH->getDirectionFlags(CurrentLocation);

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
                return (Direction) ValidDirections[RANDOM->Roll(0, ValidDirections.size() - 1)];
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
            return DIRECTION_NONE;
        }
        break;

        case PATH_BEHAVIOR_ROUTE_TO_AREA:     // Agent follows a path to the closest location in an area
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

bool MovementController::isPathReachable(MapCoordinates TestDestination)
{
    return true;
}
