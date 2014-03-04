/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package PathFinding;

import java.util.ArrayList;
import java.util.BitSet;
import java.util.concurrent.Future;
//import java.lang.Exception;

import Map.MapCoordinate;
import Map.Direction;
import Core.Dice;

/**
 *
 * @author Impaler
 */


public class Navigator {
	
	public enum MovementBehavior {
		PATH_BEHAVIOR_HALT,              // Agent stands still
		PATH_BEHAVIOR_WANDER_DRUNKENLY,  // Agent wanders and ignores all obsticles and hazards
		PATH_BEHAVIOR_WANDER_AIMLESSLY,  // Agent wanders but will not try to path through walls
		PATH_BEHAVIOR_WANDER_IN_AREA,    // Agent wanders inside a confined area
		PATH_BEHAVIOR_FOLLOW_ADJENT,     // Agent stay within a set distance of another ajent
		PATH_BEHAVIOR_FLEE_ADJENT,       // Agent runs away from another adjent
		PATH_BEHAVIOR_SEAK_ADJENT,       // Agent moves directly onto another ajent
		PATH_BEHAVIOR_ROUTE_TO_LOCATION, // Agent follows a path to a specific location
		PATH_BEHAVIOR_ROUTE_TO_AREA,     // Agent follows a path to the closest location in an area
		PATH_BEHAVIOR_TRAPPED            // Agent is trapped
	};

	Dice DirectionDice;
    PathManager ParentManager;  // The manager which spawned this controller, all data on the map and paths will come from here

    MovementBehavior CurrentMovementBehavior;

    int SizeRestriction;
    int TerrainRestriction;

    MapCoordinate CurrentLocation;
    MapCoordinate Destination;

	Future PathFuture = null;
    MapPath CurrentPath = null;
    PathWalker CurrentPathWalker = null;


	public Navigator(MapCoordinate SpawnLocation, int AjentSize, int MovementType) {
		CurrentLocation = SpawnLocation;
		Destination = SpawnLocation;

		ParentManager = PathManager.getSinglton();
		DirectionDice = new Dice();
		DirectionDice.Seed(SpawnLocation.hashCode());
	}

	public Direction getNextStep() { // Next movement step for the Agent
		switch(CurrentMovementBehavior)
		{
			case PATH_BEHAVIOR_HALT:              // Agent stands still
				return Direction.DIRECTION_NONE;

			case PATH_BEHAVIOR_WANDER_DRUNKENLY:  // Agent wanders and ignores all obsticles and hazards
				return Direction.ANGULAR_DIRECTIONS[DirectionDice.Roll(0, Direction.ANGULAR_DIRECTIONS.length - 1)];

			case PATH_BEHAVIOR_WANDER_AIMLESSLY:  // Agent wanders but will not try to path through walls
			
				BitSet DirectionFlags = ParentManager.getDirectionFlags(CurrentLocation);
				ArrayList<Integer> ValidDirections = new ArrayList<Integer>();

				for (int i = DirectionFlags.nextSetBit(0); i >= 0; i = DirectionFlags.nextSetBit(i + 1)) {
					ValidDirections.add(i);
				}

				if (ValidDirections.size() > 0) {
					return Direction.ANGULAR_DIRECTIONS[DirectionDice.Roll(0, ValidDirections.size() - 1)];
				}

				return Direction.DIRECTION_NONE;
			

			case PATH_BEHAVIOR_WANDER_IN_AREA:    // Agent wanders inside a confined area
				return Direction.DIRECTION_NONE;

			case PATH_BEHAVIOR_FOLLOW_ADJENT:     // Agent stay within a set distance of another ajent
				return Direction.DIRECTION_NONE;

			case PATH_BEHAVIOR_FLEE_ADJENT:       // Agent runs away from another adjent
				return Direction.DIRECTION_NONE;

			case PATH_BEHAVIOR_SEAK_ADJENT:       // Agent moves directly onto another ajent
				return Direction.DIRECTION_NONE;

			case PATH_BEHAVIOR_ROUTE_TO_LOCATION: // Agent follows a path to a specific location
				if (CurrentPathWalker != null) {
					return CurrentPathWalker.NextDirection();
				} else {
					try {
						if (PathFuture.isDone()) {
							CurrentPath = (MapPath) PathFuture.get();
							CurrentPathWalker = CurrentPath.getPathWalker();
						} else if (PathFuture.isCancelled()){
							//Set future to null. Maybe we succeed next time...
							PathFuture = null;
						}
					} catch(Exception e) { 
						//Exceptions.printStackTrace(e);
					}
					return Direction.DIRECTION_NONE;
				}

			case PATH_BEHAVIOR_ROUTE_TO_AREA:     // Agent follows a path to the closest location in an area
				return Direction.DIRECTION_NONE;

			case PATH_BEHAVIOR_TRAPPED:     // Agent is Trapped unable to reach a desired area
				return Direction.DIRECTION_NONE;
		}
		return Direction.DIRECTION_NONE;
	}

	public void LastStepInvalid() {  // Tell the Controller the last step order was invalid

	}

	public void setBehaviorMode(MovementBehavior NewBehavior) { // Set Crontroller to different behaviors such as wandering, halted, following, fleeing
		if (CurrentMovementBehavior != NewBehavior)
			CurrentMovementBehavior = NewBehavior;
	}

	public boolean ChangeDestination(MapCoordinate NewDestination) {
		if (NewDestination != Destination) {
			Destination = NewDestination;

			PathFuture = ParentManager.FindFuturePath(0, CurrentLocation, Destination);
			CurrentPathWalker = null;
			CurrentPath = null;
			return true;
		}
		return false;
	}

	public MapCoordinate getDestination() {
		return Destination;
	}

	public void setLocation(MapCoordinate NewLocation) {
		CurrentLocation = NewLocation;
	}

	public float getPathEstimate(MapCoordinate TestDestination) {
		return 0;
	}

	public boolean isDestinationReachable(MapCoordinate TestDestination) {
		return ParentManager.isPathPossible(0, CurrentLocation, TestDestination);
	}

}
