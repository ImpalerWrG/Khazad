#include <stdafx.h>

#include <Pawn.h>

#include <Random.h>
#include <Renderer.h>
#include <Game.h>
#include <Map.h>
#include <PathManager.h>
#include <PathTester.h>


Pawn::Pawn()
{
    CoolDown = 0;
    Moving = false;
    CurrentMovementDirection = DIRECTION_NONE;
}

Pawn::~Pawn()
{

}

bool Pawn::Init(MapCoordinates SpawnLocation)
{
	LocationCoordinates = SpawnLocation;
    RenderLocation.set(LocationCoordinates.X, LocationCoordinates.Y, LocationCoordinates.Z);
	DestinationCoordinates = SpawnLocation;  // This will imediatly trigger a pathing action

    CurrentCellCoordinates = CellCoordinates(LocationCoordinates);
	CurrentCubeCoordinates = CubeCoordinates(LocationCoordinates);
	CellActorIndex = MAP->getCell(CurrentCellCoordinates)->addActor(this);

	Controller = PATH->getNewController(0, 0, LocationCoordinates);
	Controller->setBehaviorMode(PATH_BEHAVIOR_ROUTE_TO_LOCATION);

    return true;
}

bool Pawn::BeginMove()
{
    // Set Old destinations as current Location
    MapCoordinates NewLocation = MapCoordinates(LocationCoordinates, CurrentMovementDirection);
    setLocation(NewLocation);

    Controller->setLocation(NewLocation);
    RenderLocation.set(CurrentCubeCoordinates.X, CurrentCubeCoordinates.Y, 0);
    // Delay this untill halfway point ???

    if (LocationCoordinates == DestinationCoordinates)
    {
        DestinationCoordinates = TESTER->getRandomPassableCoordinate();  // This needs to get DIFFERENT coords each time

        while (! Controller->isDestinationReachable(DestinationCoordinates))
        {
            DestinationCoordinates = TESTER->getRandomPassableCoordinate();
        }
        Controller->ChangeDestination(DestinationCoordinates);
    }

    CurrentMovementDirection = Controller->getNextStep();
    if (CurrentMovementDirection == DIRECTION_NONE)
    {
        return false;
    }

    float EdgeCost = PATH->getEdgeCost(LocationCoordinates, CurrentMovementDirection);

    if (EdgeCost != -1)
    {
        // Perform the Move
        NewLocation.TranslateMapCoordinates(CurrentMovementDirection);

        Moving = true;
        CoolDown += EdgeCost * 100;  // Cooldown factor, inverse speed

        // Create movement vector from position difference;
        RenderLocationChange.set(NewLocation.X - LocationCoordinates.X, NewLocation.Y - LocationCoordinates.Y, NewLocation.Z - LocationCoordinates.Z);
        RenderLocationChange = RenderLocationChange * (1 / (float) CoolDown);       // Reduce magnitude proportional to cooldown

        return true;
    }
    return false;
}

int Pawn::Update()
{
    if (!CoolDown--)
    {
        Moving = BeginMove();
    }

    if (Moving)
    {
        UpdateRenderPosition();
    }

    return 1;
}

bool Pawn::UpdateRenderPosition()
{
    RenderLocation += RenderLocationChange;

    return true;  // Adjust Position vector incrementaly her to make smoth movements
}

bool Pawn::Draw(CameraOrientation Orientaion)
{
    RENDERER->DrawDiamond(RenderLocation.x, RenderLocation.y, RenderLocation.z, 1, 1, 1, 1);

    return true;
}
