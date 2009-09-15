#ifndef PAWN__HEADER
#define PAWN__HEADER

#include <stdafx.h>

#include <Actor.h>

class Cell;

class Pawn: public Actor
{

public:

    Pawn();
	~Pawn();
    bool Init(Vector3 SpawnLocation);

	Cell* getCell() {return Location;}

	bool Move(Vector3 Direction);
	bool Update();
	bool Draw();


protected:

	Cell* Location;

};

#endif // PAWN__HEADER
