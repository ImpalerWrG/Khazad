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

#include <Pawn.h>

#include <DataManager.h>
#include <Game.h>
#include <Map.h>

#include <TextureManager.h>

#include <PathManager.h>
#include <PathTester.h>


Pawn::Pawn()
{
    Moving = false;
    Controller = NULL;
    CurrentMovementDirection = DIRECTION_NONE;
}

Pawn::~Pawn()
{

}

bool Pawn::Init(MapCoordinates SpawnLocation)
{
    Ogre::MaterialPtr MatPointer = TEXTURE->makeAnimatedMaterial(DATA->getLabelIndex("ANIMATION_CAT"), DATA->getLabelIndex("COLOR_BROWN"));

    Actor::Init(SpawnLocation, MatPointer, 1.0, 1.5);

    Controller = GAME->getPath()->getNewController(0, 0, LocationCoordinates);

    DestinationCoordinates = LocationCoordinates;

	Controller->setBehaviorMode(PATH_BEHAVIOR_ROUTE_TO_LOCATION);

	AnimationGroupID = DATA->getLabelIndex("ANIMATION_CAT");
	setAnimationType(DATA->getLabelIndex("ANIMATION_TYPE_IDLE"));

	//Ogre::TextureUnitState* TexUnit = Mat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
	//TexUnit->setCurrentFrame(8);
	//Controller->setBehaviorMode(PATH_BEHAVIOR_WANDER_AIMLESSLY);

    return true;
}

CoolDown Pawn::AttemptMove(Direction MovementDirection)
{
    float EdgeCost = GAME->getPath()->getEdgeCost(LocationCoordinates, MovementDirection);

    if (EdgeCost != -1)
    {
        MapCoordinates NewLocation = MapCoordinates(LocationCoordinates, MovementDirection);

        Moving = true;
        float Speed = 1.0;
        MovementCoolDown = EdgeCost / (Speed / 1000.0);
        MovementStarted = TEMPORAL->getCurrentTimeTick();

        float AnimationSpeed = 0.5;
        float FramesToMove = AnimationLoopLength * AnimationSpeed * EdgeCost;
        FrameCooldown = MovementCoolDown / FramesToMove;

        // Create Vector directly from a MovementDirection?
        RenderLocationChange.x = NewLocation.X - LocationCoordinates.X;
        RenderLocationChange.y = NewLocation.Y - LocationCoordinates.Y;
        RenderLocationChange.z = NewLocation.Z - LocationCoordinates.Z;

        RenderLocationChange = RenderLocationChange / FramesToMove;
        return FrameCooldown;
        // Reduce magnitude proportional to cooldown
    } else {
    	Moving = false;
    	// Unexpected obstacle
    }
    return 1;
}

void Pawn::setAnimationType(ANIMATION_TYPE_INDEX NewAnimationType)
{
    AnimationStartIndex = CurrentFrame = DATA->getAnimationGroupData(AnimationGroupID)->getAnimationStart(NewAnimationType);
	AnimationLoopLength = DATA->getAnimationGroupData(AnimationGroupID)->getAnimationLength(NewAnimationType);

	//ActorBillboard->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setCurrentFrame(CurrentFrame);
}

void Pawn::AdvanceFrame()
{
    ActorBillboard->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setCurrentFrame(CurrentFrame++);  // advance the animation

    if (CurrentFrame == AnimationLoopLength)
    {
        CurrentFrame = AnimationStartIndex;
    }
}

CoolDown Pawn::Update()
{
    UpdateTick = TEMPORAL->getCurrentTimeTick();   // Record the current Tick


    if (Moving)
    {
        //AdvanceFrame();
        MoveRenderPosition(RenderLocationChange);

        if (UpdateTick >= (MovementStarted + MovementCoolDown))  // Done
        {
            MapCoordinates NewLocation = MapCoordinates(LocationCoordinates, CurrentMovementDirection);
            setLocation(NewLocation);
            Controller->setLocation(NewLocation);
            Moving = false;
        }
        return FrameCooldown;
    }
    else
    {
        if (LocationCoordinates == DestinationCoordinates)
        {
            if (GAME->getPath()->getDirectionFlags(LocationCoordinates) != 0) //Is the current location impassable
            {
                DestinationCoordinates = GAME->getPath()->getTester()->getRandomPassableCoordinate();  // This needs to get DIFFERENT coords each time

                while (!Controller->isDestinationReachable(DestinationCoordinates))
                {
                    DestinationCoordinates = GAME->getPath()->getTester()->getRandomPassableCoordinate();
                }
                Controller->ChangeDestination(DestinationCoordinates);
            } else {
                Controller->setBehaviorMode(PATH_BEHAVIOR_HALT);
                Moving = false;
                return 100;
            }
        }

        CurrentMovementDirection = Controller->getNextStep();

        if (CurrentMovementDirection != DIRECTION_NONE)
        {
            return AttemptMove(CurrentMovementDirection);
        }
    }

    return 1;
}

Ogre::Vector3 Pawn::getRenderPosition()
{
    return Ogre::Vector3(LocationCoordinates.X, LocationCoordinates.Y, LocationCoordinates.Z) + getRederPositionMovementAdjustment();
}

Ogre::Vector3 Pawn::getRederPositionMovementAdjustment()
{
    uint32_t MovementMultiplier = TEMPORAL->getCurrentTimeTick() - UpdateTick;
    return RenderLocationChange * MovementMultiplier;
}

