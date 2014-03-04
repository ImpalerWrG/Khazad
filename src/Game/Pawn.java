/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Game;

import com.jme3.math.Vector3f;

import Map.Direction;
import Map.MapCoordinate;

import PathFinding.Navigator;
import PathFinding.PathManager;
/**
 *
 * @author Impaler
 */
public class Pawn extends Actor {

	boolean Moving;
	Direction CurrentMovementDirection;

	//float FrameCooldown;
	int MovementDuration;
	long MovementStarted;
	//long UpdateTick;

	MapCoordinate DestinationCoordinates;

	//Ogre::Vector3 RenderLocation;
	Vector3f RenderLocationChange;

	//uint16_t AnimationGroupID;
	//uint16_t AnimationStartIndex;
	//uint16_t CurrentFrame;
	//uint16_t AnimationLoopLength;

	Navigator PathNavigator;

	byte Strength = 0;
	byte Dextarity = 0;
	byte Flexibility = 0;
	byte Endurance = 0;
	byte Vitality = 0;
	byte Reflexes = 0;
	byte Speed = 0;

	byte Logic = 0;
	byte Perception = 0;
	byte Charisma = 0;
	byte Memory = 0;
	byte Judgement = 0;
	byte Willpower = 0;
	byte Intuition = 0;

	Pawn(int id, MapCoordinate SpawnLocation) {
		super(id, SpawnLocation);		
		Moving = false;
		PathNavigator = new Navigator(SpawnLocation, 1, 1);
		PathNavigator.setBehaviorMode(Navigator.MovementBehavior.PATH_BEHAVIOR_ROUTE_TO_LOCATION);

		CurrentMovementDirection = Direction.DIRECTION_NONE;
		RenderLocationChange = new Vector3f();
		
		Speed = 1;
		RandomRoute();
	}
	
	int AttemptMove(Direction MovementDirection) {
		float EdgeCost = PathManager.getSinglton().getEdgeCost(LocationCoordinates, MovementDirection);

		if (EdgeCost != -1) {
			MapCoordinate NewLocation = new MapCoordinate(LocationCoordinates, MovementDirection);

			Moving = true;
			MovementDuration = (int) (EdgeCost / Speed);
			MovementStarted = TemporalManager.getSingleton().getCurrentTimeTick();

			// Create Vector directly from a MovementDirection?
			RenderLocationChange.x = NewLocation.X - LocationCoordinates.X;
			RenderLocationChange.y = NewLocation.Y - LocationCoordinates.Y;
			RenderLocationChange.z = NewLocation.Z - LocationCoordinates.Z;

			RenderLocationChange = RenderLocationChange.divide(MovementDuration);
			return MovementDuration;
			// Reduce magnitude proportional to cooldown
		} else {
			Moving = false;
			// Unexpected obstacle
		}
		return 1;
	}

	//void setAnimationType(ANIMATION_TYPE_INDEX NewAnimationType) {
		//AnimationStartIndex = CurrentFrame = DATA->getAnimationGroupData(AnimationGroupID)->getAnimationStart(NewAnimationType);
		//AnimationLoopLength = DATA->getAnimationGroupData(AnimationGroupID)->getAnimationLength(NewAnimationType);

		//ActorBillboard->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setCurrentFrame(CurrentFrame);
	//}

	void AdvanceFrame() {
		//ActorBillboard->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setCurrentFrame(CurrentFrame++);  // advance the animation

		//if (CurrentFrame == AnimationLoopLength) {
		//	CurrentFrame = AnimationStartIndex;
		//}
	}

	int UpdatePosition() {
		TemporalManager Time = TemporalManager.getSingleton();
		long CurrentTick = Time.getCurrentTimeTick();   // Record the current Tick

		if (Moving) {
			if (CurrentTick >= (MovementStarted + MovementDuration)) { // Done
				MapCoordinate NewLocation = new MapCoordinate(LocationCoordinates, CurrentMovementDirection);
				setLocation(NewLocation);
				Moving = false;
			}
			return (int) (MovementDuration - (CurrentTick - MovementStarted));
		} else {
			CurrentMovementDirection = PathNavigator.getNextStep();
			if (CurrentMovementDirection != Direction.DIRECTION_NONE) {
				if (CurrentMovementDirection != Direction.DIRECTION_DESTINATION) {
					return AttemptMove(CurrentMovementDirection);
				} else {
					RandomRoute();
				}
			}
			return 1;
		}
	}

	public void RandomRoute() {
		PathManager Pathing = PathManager.getSinglton();
		if (Pathing.getDirectionFlags(LocationCoordinates).cardinality() != 0) {//Is the current location impassable
			DestinationCoordinates = Pathing.Tester.getRandomPassableCoordinate();  // This needs to get DIFFERENT coords each time

			while (!PathNavigator.isDestinationReachable(DestinationCoordinates)) {
				DestinationCoordinates = Pathing.Tester.getRandomPassableCoordinate();
			}
			PathNavigator.ChangeDestination(DestinationCoordinates);
		} else {
			PathNavigator.setBehaviorMode(Navigator.MovementBehavior.PATH_BEHAVIOR_HALT);
			Moving = false;
		}
	}

	@Override
	public void setLocation(MapCoordinate NewLocation) {
		super.setLocation(NewLocation);
		PathNavigator.setLocation(NewLocation);
	}

	//Vector3f getRenderPosition() {
	//	return new Vector3f(LocationCoordinates.X, LocationCoordinates.Y, LocationCoordinates.Z).add(getRederPositionMovementAdjustment());
	//}

	//Vector3f getRederPositionMovementAdjustment() {
	//	long MovementMultiplier = TemporalManager.getSingleton().getCurrentTimeTick() - UpdateTick;
	//	return RenderLocationChange.mult(MovementMultiplier);
	//}
	
	@Override
	int Wake() {
		super.Wake();
		return UpdatePosition();
	}
}
