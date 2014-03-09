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

	int MovementDuration = 1;
	long MovementStarted;

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

	public Pawn(int id, MapCoordinate SpawnLocation) {
		super(id, SpawnLocation);		
		Moving = false;
		PathNavigator = new Navigator(SpawnLocation, 1, 1);
		PathNavigator.setBehaviorMode(Navigator.MovementBehavior.PATH_BEHAVIOR_ROUTE_TO_LOCATION);

		CurrentMovementDirection = Direction.DIRECTION_NONE;
		
		Speed = 5;  // 5 is normal dwarven speed
		RandomRoute();
	}
	
	int AttemptMove(Direction MovementDirection) {
		float EdgeCost = PathManager.getSinglton().getEdgeCost(LocationCoordinates, MovementDirection);

		if (EdgeCost != -1) {
			MapCoordinate NewLocation = new MapCoordinate(LocationCoordinates, MovementDirection);

			Moving = true;
			MovementDuration = (int) (EdgeCost / (Speed / 5) * 12);
			MovementStarted = TemporalManager.getSingleton().getCurrentTimeTick();

			return MovementDuration;
			// Reduce magnitude proportional to cooldown
		} else {
			Moving = false;
			// Unexpected obstacle
		}
		return 1;
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
			MapCoordinate DestinationCoordinates = Pathing.Tester.getRandomPassableCoordinate();  // This needs to get DIFFERENT coords each time

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

	public float getMovementFraction() {
		long CurrentTick = TemporalManager.getSingleton().getCurrentTimeTick();
		return (CurrentTick - MovementStarted) / ((float) MovementDuration);
	}
	
	public Direction getMovementDirection() {
		return CurrentMovementDirection;
	}
	
	@Override
	int Wake() {
		super.Wake();
		return UpdatePosition();
	}
}
