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

	bool Move();
	bool Update();
    bool Draw(CameraOrientation Orientaion);

protected:

	Cell* CellLocation;

    MovementController* Controller;
};

#endif // PAWN__HEADER
