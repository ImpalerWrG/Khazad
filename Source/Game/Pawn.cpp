#include <Pawn.h>

//#include <Random.h>
//#include <Renderer.h>
#include <DataManager.h>
#include <Game.h>
#include <Map.h>

#include <TextureManager.h>

//#include <PathManager.h>
//#include <PathTester.h>


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

    //Controller = PATH->getNewController(0, 0, LocationCoordinates);
	//Controller->setBehaviorMode(PATH_BEHAVIOR_ROUTE_TO_LOCATION);

    return true;
}

int Pawn::AttemptMove(Direction MovementDirection)
{
    float EdgeCost = 0;  //PATH->getEdgeCost(LocationCoordinates, MovementDirection);

    if (EdgeCost != -1)
    {
        MapCoordinates NewLocation = MapCoordinates(LocationCoordinates, MovementDirection);

        Moving = true;
        CoolDown MovementCoolDown = EdgeCost * 100;  // Cooldown factor, inverse speed

        // Create Vector directly from a MovementDirection?
        //RenderLocationChange.set(NewLocation.X - LocationCoordinates.X, NewLocation.Y - LocationCoordinates.Y, NewLocation.Z - LocationCoordinates.Z);
        //RenderLocationChange = RenderLocationChange * (1 / (float) CoolDown);
        // Reduce magnitude proportional to cooldown

        return MovementCoolDown;
    }
    return 1;
}

CoolDown Pawn::Update()
{
    UpdateTick = GAME->getTickCount();   // Record the current Tick

    if (Moving)
    {
        // Set Old destinations as current Location
        // Delay this untill halfway point ???
        MapCoordinates NewLocation = MapCoordinates(LocationCoordinates, CurrentMovementDirection);
        setLocation(NewLocation);

        //Controller->setLocation(NewLocation);
    }

    if (LocationCoordinates == DestinationCoordinates)
    {
        //DestinationCoordinates = TESTER->getRandomPassableCoordinate();  // This needs to get DIFFERENT coords each time

        //while (!Controller->isDestinationReachable(DestinationCoordinates))
        //{
        //    DestinationCoordinates = TESTER->getRandomPassableCoordinate();
        //}
        //Controller->ChangeDestination(DestinationCoordinates);
    }

    //CurrentMovementDirection = Controller->getNextStep();

    if (CurrentMovementDirection != DIRECTION_NONE)
    {
        return AttemptMove(CurrentMovementDirection);
    }

    return 1;
}

Ogre::Vector3 Pawn::getRenderPosition()
{
    return Ogre::Vector3(LocationCoordinates.X, LocationCoordinates.Y, LocationCoordinates.Z) + getRederPositionMovementAdjustment();
}

Ogre::Vector3 Pawn::getRederPositionMovementAdjustment()
{
    uint32_t MovementMultiplier = GAME->getTickCount() - UpdateTick;
    return RenderLocationChange * MovementMultiplier;
}

/*
bool Pawn::Draw(CameraOrientation Orientaion)
{
    Vector3 MovmentAdjustment = getRederPositionMovementAdjustment();

    RENDERER->DrawDiamond(CurrentCubeCoordinates.X + MovmentAdjustment.x, CurrentCubeCoordinates.Y + MovmentAdjustment.y, MovmentAdjustment.z, 1, 1, 1, 1);

    //MapCoordinates CageCoordinates = MapCoordinates(CurrentCubeCoordinates.X, CurrentCubeCoordinates.Y, 0);
    //RENDERER->DrawCage(CageCoordinates, 1, 1, 1, false, 1, 1, 1);

    return true;
}
*/
