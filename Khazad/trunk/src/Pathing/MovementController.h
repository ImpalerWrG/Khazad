#ifndef MOVE_CONTROLLER__HEADER
#define MOVE_CONTROLLER__HEADER


#include <Coordinates.h>
#include <Path.h>

class PathManager;

enum MovementBehavior
{
    PATH_BEHAVIOR_HALT,              // Agent stands still
    PATH_BEHAVIOR_WANDER_DRUNKENLY,  // Agent wanders and ignores all obsticles and hazards
    PATH_BEHAVIOR_WANDER_AIMLESSLY,  // Agent wanders but will not try to path through walls
    PATH_BEHAVIOR_WANDER_IN_AREA,    // Agent wanders inside a confined area
    PATH_BEHAVIOR_FOLLOW_ADJENT,     // Agent stay within a set distance of another ajent
    PATH_BEHAVIOR_FLEE_ADJENT,       // Agent runs away from another adjent
    PATH_BEHAVIOR_SEAK_ADJENT,       // Agent moves directly onto another ajent
    PATH_BEHAVIOR_ROUTE_TO_LOCATION, // Agent follows a path to a specific location
    PATH_BEHAVIOR_ROUTE_TO_AREA      // Agent follows a path to the closest location in an area
};

class MovementController
{
public:

    MovementController(int AjentSize, int MovementType);
    ~MovementController();

    Direction getNextStep();  // Next movement step for the ajent
    void LastStepInvalid();  // Tell the Controller the last step order was invalid

    void setBehaviorMode(MovementBehavior NewBehavior); // Set Crontroller to different behaviors such as wandering, halted, following, fleeing

    bool ChangeDestination(MapCoordinates NewDestination);
    void setLocation(MapCoordinates NewLocation);

    float getPathEstimate(MapCoordinates TestDestination);
    bool isPathReachable(MapCoordinates TestDestination);

private:

    PathManager* ParentManager;  // The manager which spawned this controller, all data on the map and paths will come from here

    MovementBehavior CurrentMovementBehavior;

    int SizeRestriction;
    int TerrainRestriction;

    MapCoordinates CurrentLocation;
    MapCoordinates Destination;
};

#endif // MOVE_CONTROLLER__HEADER
