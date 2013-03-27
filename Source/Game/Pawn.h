/* Copyright 2010 Kenneth Ferland

This file is part of Khazad.

Khazad is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Khazad is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Khazad.  If not, see <http://www.gnu.org/licenses/> */

/*-------------------------PAWN----------------------------

Pawn is a virtual baseclass for any mobile creature in the
game world.  It obviously derives from Actor and adds the capability
of movement as well as providing the link to the Pathfinding engine
through the inclusion of the Movement Controller object which
provides all movement direction logic.

Pawn also implements the 14 attributes of the character system. */


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
