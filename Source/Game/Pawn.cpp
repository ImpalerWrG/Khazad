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
    CurrentMovementDirection = DIRECTION_NONE;
}

Pawn::~Pawn()
{

}

bool Pawn::Init(MapCoordinates SpawnLocation)
{
    Actor::Init(SpawnLocation);

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

int Pawn::AttemptMove(Direction MovementDirection)
{
    float EdgeCost = GAME->getPath()->getEdgeCost(LocationCoordinates, MovementDirection);

    if (EdgeCost != -1)
    {
        MapCoordinates NewLocation = MapCoordinates(LocationCoordinates, MovementDirection);

        Moving = true;
        MovementCoolDown = EdgeCost * 1000;  // Cooldown factor, inverse speed
        MovementStarted = TEMPORAL->getCurrentTimeTick();

        // Create Vector directly from a MovementDirection?
        RenderLocationChange.x = NewLocation.X - LocationCoordinates.X;
        RenderLocationChange.y = NewLocation.Y - LocationCoordinates.Y;
        RenderLocationChange.z = NewLocation.Z - LocationCoordinates.Z;

        RenderLocationChange = RenderLocationChange * (1 / (float) MovementCoolDown);
        // Reduce magnitude proportional to cooldown
    }
    return 1;
}

void Pawn::setAnimationType(ANIMATION_TYPE_INDEX NewAnimationType)
{
    AnimationStartIndex = CurrentFrame = DATA->getAnimationGroupData(AnimationGroupID)->getAnimationStart(NewAnimationType);
	AnimationLoopLength = DATA->getAnimationGroupData(AnimationGroupID)->getAnimationLength(NewAnimationType);

	Mat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setCurrentFrame(CurrentFrame);
}

void Pawn::AdvanceFrame()
{
    Mat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setCurrentFrame(CurrentFrame++);  // advance the animation

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
        MoveRenderPosition(RenderLocationChange);

        if (UpdateTick > (MovementStarted + MovementCoolDown))  // Done
        {
            MapCoordinates NewLocation = MapCoordinates(LocationCoordinates, CurrentMovementDirection);
            setLocation(NewLocation);
            Controller->setLocation(NewLocation);

            Moving = false;
        }
    }
    else
    {
        if (LocationCoordinates == DestinationCoordinates)
        {
            DestinationCoordinates = GAME->getPath()->getTester()->getRandomPassableCoordinate();  // This needs to get DIFFERENT coords each time

            while (!Controller->isDestinationReachable(DestinationCoordinates))
            {
                DestinationCoordinates = GAME->getPath()->getTester()->getRandomPassableCoordinate();
            }
            Controller->ChangeDestination(DestinationCoordinates);
        }

        CurrentMovementDirection = Controller->getNextStep();
        AdvanceFrame();

        if (CurrentMovementDirection != DIRECTION_NONE)
        {
            //AdvanceFrame();
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

