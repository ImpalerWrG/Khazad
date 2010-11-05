#include <Pawn.h>

#include <DataManager.h>
#include <Game.h>
#include <Map.h>

#include <TextureManager.h>

#include <PathManager.h>
#include <PathTester.h>


Pawn::Pawn()
{
    Moving = false;
    CurrentMovementDirection = DIRECTION_NONE;
}

Pawn::~Pawn()
{

}

bool Pawn::Init(MapCoordinates SpawnLocation)
{
    Actor::Init(SpawnLocation);

    Controller = GAME->getPath()->getNewController(0, 0, LocationCoordinates);

    DestinationCoordinates = LocationCoordinates;

	Controller->setBehaviorMode(PATH_BEHAVIOR_ROUTE_TO_LOCATION);
	//Controller->setBehaviorMode(PATH_BEHAVIOR_WANDER_AIMLESSLY);

    return true;
}

int Pawn::AttemptMove(Direction MovementDirection)
{
    float EdgeCost = GAME->getPath()->getEdgeCost(LocationCoordinates, MovementDirection);

    if (EdgeCost != -1)
    {
        MapCoordinates NewLocation = MapCoordinates(LocationCoordinates, MovementDirection);

        Moving = true;
        MovementCoolDown = EdgeCost * 1000;  // Cooldown factor, inverse speed
        MovementStarted = TEMPORAL->getCurrentTimeTick();

        // Create Vector directly from a MovementDirection?
        RenderLocationChange.x = NewLocation.X - LocationCoordinates.X;
        RenderLocationChange.y = NewLocation.Y - LocationCoordinates.Y;
        RenderLocationChange.z = NewLocation.Z - LocationCoordinates.Z;

        RenderLocationChange = RenderLocationChange * (1 / (float) MovementCoolDown);
        // Reduce magnitude proportional to cooldown
    }
    return 1;
}

CoolDown Pawn::Update()
{
    UpdateTick = TEMPORAL->getCurrentTimeTick();   // Record the current Tick

    if (Moving)
    {
        MoveRenderPosition(RenderLocationChange);

        //if (UpdateTick > (MovementStarted + (MovementCoolDown / 2)))  // Halfway point? move for logic purposes/collision etc

        if (UpdateTick > (MovementStarted + MovementCoolDown))  // Done
        {
            MapCoordinates NewLocation = MapCoordinates(LocationCoordinates, CurrentMovementDirection);
            setLocation(NewLocation);
            Controller->setLocation(NewLocation);

            Moving = false;
        }
    }
    else
    {
        if (LocationCoordinates == DestinationCoordinates)
        {
            DestinationCoordinates = GAME->getPath()->getTester()->getRandomPassableCoordinate();  // This needs to get DIFFERENT coords each time

            while (!Controller->isDestinationReachable(DestinationCoordinates))
            {
                DestinationCoordinates = GAME->getPath()->getTester()->getRandomPassableCoordinate();
            }
            Controller->ChangeDestination(DestinationCoordinates);
        }

        CurrentMovementDirection = Controller->getNextStep();

        if (CurrentMovementDirection != DIRECTION_NONE)
        {
            return AttemptMove(CurrentMovementDirection);
        }
    }

    return 1;
}

Ogre::Vector3 Pawn::getRenderPosition()
{
    return Ogre::Vector3(LocationCoordinates.X, LocationCoordinates.Y, LocationCoordinates.Z) + getRederPositionMovementAdjustment();
}

Ogre::Vector3 Pawn::getRederPositionMovementAdjustment()
{
    uint32_t MovementMultiplier = TEMPORAL->getCurrentTimeTick() - UpdateTick;
    return RenderLocationChange * MovementMultiplier;
}

