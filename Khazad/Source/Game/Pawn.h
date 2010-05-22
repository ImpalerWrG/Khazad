#ifndef PAWN__HEADER
#define PAWN__HEADER

#include <stdafx.h>

#include <Actor.h>
#include <Coordinates.h>
#include <OgreVector3.h>
//#include <MovementController.h>

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

    Ogre::Vector3 getRenderPosition();
    Ogre::Vector3 getRederPositionMovementAdjustment();
    //bool Draw(CameraOrientation Orientaion);

protected:

	Cell* CellLocation;

	bool Moving;
	Direction CurrentMovementDirection;

	int CoolDown;
	int UpdateTick;

    MapCoordinates DestinationCoordinates;

	Ogre::Vector3 RenderLocation;
	Ogre::Vector3 RenderLocationChange;

    //MovementController* Controller;
};

#endif // PAWN__HEADER
