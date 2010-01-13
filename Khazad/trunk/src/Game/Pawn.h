#ifndef PAWN__HEADER
#define PAWN__HEADER

#include <stdafx.h>

#include <Actor.h>
#include <Coordinates.h>
#include <MovementController.h>

class Cell;

class Pawn: public Actor
{

public:

    Pawn();
	~Pawn();

    bool Init(MapCoordinates SpawnCoordinates);

	Cell* getCell() { return CellLocation; }

	bool BeginMove();
	int Update();
	bool UpdateRenderPosition();
    bool Draw(CameraOrientation Orientaion);

protected:

	Cell* CellLocation;
	CellCoordinates CurrentCell;

	bool Moving;
	Direction CurrentMovementDirection;
	int CoolDown;

    MapCoordinates DestinationCoordinates;

	Vector3 RenderLocation;
	Vector3 RenderLocationChange;

    MovementController* Controller;
};

#endif // PAWN__HEADER
