#ifndef PAWN__HEADER
#define PAWN__HEADER

#include <stdafx.h>

#include <DataManager.h>
#include <Actor.h>
#include <Coordinates.h>
#include <OgreVector3.h>
#include <MovementController.h>

class Cell;

class Pawn: virtual public Actor
{

public:

    Pawn();
	~Pawn();

    bool Init(MapCoordinates SpawnCoordinates);

	Cell* getCell() { return CellLocation; }

    void AdvanceFrame();
	CoolDown AttemptMove(Direction MovementDirection);
	CoolDown Update();

    void setAnimationType(ANIMATION_TYPE_INDEX NewAnimationType);

    Ogre::Vector3 getRenderPosition();
    Ogre::Vector3 getRederPositionMovementAdjustment();
    //bool Draw(CameraOrientation Orientaion);

protected:

	Cell* CellLocation;

	bool Moving;
	Direction CurrentMovementDirection;

    CoolDown FrameCooldown;
	CoolDown MovementCoolDown;
	Tick MovementStarted;
	Tick UpdateTick;

    MapCoordinates DestinationCoordinates;

	Ogre::Vector3 RenderLocation;
	Ogre::Vector3 RenderLocationChange;

	uint16_t AnimationGroupID;
	uint16_t AnimationStartIndex;
	uint16_t CurrentFrame;
	uint16_t AnimationLoopLength;

    MovementController* Controller;
};

#endif // PAWN__HEADER
