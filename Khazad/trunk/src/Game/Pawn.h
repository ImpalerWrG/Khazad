#ifndef PAWN__HEADER
#define PAWN__HEADER

#include <stdafx.h>

#include <Actor.h>
#include <Coordinates.h>
#include <Vector3.h>
#include <MovementController.h>

class Cell;

class Pawn: public Actor
{

public:

    Pawn();
	~Pawn();

    bool Init(MapCoordinates SpawnCoordinates);

	Cell* getCell() { return CellLocation; }

	int AttemptMove(Direction MovementDirection);
	int Update();

    Vector3 getRenderPosition();
    Vector3 getRederPositionMovementAdjustment();
    bool Draw(CameraOrientation Orientaion);

protected:

	Cell* CellLocation;

	bool Moving;
	Direction CurrentMovementDirection;

	int CoolDown;
	int UpdateTick;

    MapCoordinates DestinationCoordinates;

	Vector3 RenderLocation;
	Vector3 RenderLocationChange;

    MovementController* Controller;
};

#endif // PAWN__HEADER
