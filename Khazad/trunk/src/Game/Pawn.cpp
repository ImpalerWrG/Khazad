#include <stdafx.h>

#include <Pawn.h>

#include <Random.h>
#include <Renderer.h>
#include <Game.h>
#include <Map.h>
#include <PathManager.h>


Pawn::Pawn()
{
    CoolDown = 0;
    Moving = false;
}

Pawn::~Pawn()
{

}

bool Pawn::Init(MapCoordinates SpawnLocation)
{
	LocationCoordinates = SpawnLocation;
	Controller = PATH->getNewController(0, 0, LocationCoordinates);
	Controller->setBehaviorMode(PATH_BEHAVIOR_WANDER_AIMLESSLY);

    return true;
}

bool Pawn::BeginMove()
{
    Direction MoveDirection = Controller->getNextStep();

    if (MoveDirection == DIRECTION_NONE)
    {
        return false;
    }

    float EdgeCost = PATH->getEdgeCost(LocationCoordinates, MoveDirection);

    if (EdgeCost != -1)
    {
        // Perform the Move
        RenderLocation.set(LocationCoordinates.X, LocationCoordinates.Y, LocationCoordinates.Z);

        LocationCoordinates.TranslateMapCoordinates(MoveDirection);
        Controller->setLocation(LocationCoordinates);  // Delay this untill halfway point

        Moving = true;
        CoolDown += EdgeCost * 10;

        RenderLocationChange.set(LocationCoordinates.X, LocationCoordinates.Y, LocationCoordinates.Z);
        RenderLocationChange -= RenderLocation;  // Create movement vector from position difference;
        RenderLocationChange = RenderLocationChange * (1 / (float) CoolDown);       // Reduce magnitude proportional to cooldown

        return true;
    }
    return false;
}

bool Pawn::Update()
{
    if (!CoolDown--)
    {
        Moving = BeginMove();
    }

    if (Moving)
    {
        UpdateRenderPosition();
    }

    return true;
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
