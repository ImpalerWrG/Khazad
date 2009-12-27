#include <stdafx.h>

#include <Pawn.h>

#include <Random.h>
#include <Renderer.h>
#include <Game.h>
#include <Map.h>
#include <PathManager.h>


Pawn::Pawn()
{

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

bool Pawn::Move()
{
    Direction MoveDirection = Controller->getNextStep();

    if (PATH->getEdgeCost(LocationCoordinates, MoveDirection) != -1)
    {
        // Perform the Move
        LocationCoordinates.TranslateMapCoordinates(MoveDirection);
        Controller->setLocation(LocationCoordinates);
    }

    return true;
}

bool Pawn::Update()
{
    Move();

    return true;
}

bool Pawn::Draw(CameraOrientation Orientaion)
{
    RENDERER->DrawDiamond(LocationCoordinates, 1, 1, 1, 1);

    return true;
}
