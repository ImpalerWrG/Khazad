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

    uint8_t Strength;
    uint8_t Dextarity;
    uint8_t Flexibility;
    uint8_t Endurance;
    uint8_t Vitality;
    uint8_t Reflexes;
    uint8_t Speed;

    uint8_t Logic;
    uint8_t Perception;
    uint8_t Charisma;
    uint8_t Memory;
    uint8_t Judgement;
    uint8_t Willpower;
    uint8_t Intuition;
};

#endif // PAWN__HEADER
