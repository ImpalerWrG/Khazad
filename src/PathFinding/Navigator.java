/* Copyright 2010 Kenneth 'Impaler' Ferland

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
 along with Khazad.	If not, see <http://www.gnu.org/licenses/> */

package PathFinding;

import java.util.ArrayList;
import java.util.BitSet;
import java.util.concurrent.Future;

import Map.Coordinates.MapCoordinate;
import Map.Coordinates.Direction;
import Core.Dice;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.Serializable;

/**
 * Navigator acts as the interface between Pawns (moving things) and the Pathfinding
 * Engine, the Pawn needs to set the MovementBehavior that it wishes to conduct
 * and then query for the next Direction to move in. When pathing to a destination
 * is selected and a destination set, the Navigator will call the Pathfinding
 * Engine to find the path and will then instantiate a PathWalker which will be
 * querried for Directions to provide to the Pawn.
 *
 * @author Impaler
 */
public class Navigator implements Serializable {

	private static final long serialVersionUID = 1;

	public enum MovementBehavior {

		PATH_BEHAVIOR_HALT, // Agent stands still
		PATH_BEHAVIOR_WANDER_DRUNKENLY, // Agent wanders and ignores all obstacles and hazards
		PATH_BEHAVIOR_WANDER_AIMLESSLY, // Agent wanders but will not try to path through walls
		PATH_BEHAVIOR_WANDER_IN_AREA, // Agent wanders inside a confined area
		PATH_BEHAVIOR_FOLLOW_PAWN, // Agent stay within a set distance of another pawn
		PATH_BEHAVIOR_FLEE_PAWN, // Agent runs away from another pawn
		PATH_BEHAVIOR_SEAK_PAWN, // Agent moves directly onto another pawn
		PATH_BEHAVIOR_ROUTE_TO_LOCATION, // Agent follows a path to a specific location
		PATH_BEHAVIOR_ROUTE_TO_AREA, // Agent follows a path to the closest location in an area
		PATH_BEHAVIOR_TRAPPED				// Agent is trapped
	};

	Dice DirectionDice;
	transient PathManager ParentManager;	// The manager which spawned this controller, all data on the map and paths will come from here
	MovementBehavior CurrentMovementBehavior;
	MovementModality Modality;
	int ModalityGridIndex;

	MapCoordinate CurrentLocation;
	MapCoordinate Destination;

	transient Future PathFuture = null;
	MapPath CurrentPath = null;
	PathWalker CurrentPathWalker = null;

	public Navigator(MapCoordinate SpawnLocation, MovementModality MovementType) {
		ParentManager = PathManager.getSingleton();
		CurrentLocation = SpawnLocation;
		Destination = SpawnLocation;
		Modality = MovementType;
		ModalityGridIndex = ParentManager.getModalityGridIndex(Modality);

		DirectionDice = new Dice();
		DirectionDice.seed(SpawnLocation.hashCode());
	}

	// this method is used by serialization
	private void readObject(ObjectInputStream ois) throws ClassNotFoundException, IOException {
		// default deserialization
		ois.defaultReadObject();
		// fix transients
		ParentManager = PathManager.getSingleton();
		PathFuture = ParentManager.findFuturePath(Modality, CurrentLocation, Destination);
	}

	public Direction getNextStep() { // Next movement step for the Agent
		switch (CurrentMovementBehavior) {
			case PATH_BEHAVIOR_HALT:		// Agent stands still
				return Direction.DIRECTION_NONE;

			case PATH_BEHAVIOR_WANDER_DRUNKENLY:	// Agent wanders and ignores all obstacles and hazards
				return Direction.ANGULAR_DIRECTIONS[DirectionDice.roll(0, Direction.ANGULAR_DIRECTIONS.length - 1)];

			case PATH_BEHAVIOR_WANDER_AIMLESSLY:	// Agent wanders but will not try to path through walls

				BitSet DirectionFlags = ParentManager.getDirectionFlags(CurrentLocation, Modality);
				ArrayList<Integer> ValidDirections = new ArrayList<Integer>();

				for (int i = DirectionFlags.nextSetBit(0); i >= 0; i = DirectionFlags.nextSetBit(i + 1)) {
					ValidDirections.add(i);
				}

				if (ValidDirections.size() > 0) {
					int Index = ValidDirections.get(DirectionDice.roll(0, ValidDirections.size() - 1));
					return Direction.ANGULAR_DIRECTIONS[Index];
				}

				return Direction.DIRECTION_NONE;

			case PATH_BEHAVIOR_WANDER_IN_AREA:	  // Agent wanders inside a confined area
				return Direction.DIRECTION_NONE;

			case PATH_BEHAVIOR_FOLLOW_PAWN:		// Agent stay within a set distance of another pawn
				return Direction.DIRECTION_NONE;

			case PATH_BEHAVIOR_FLEE_PAWN:		// Agent runs away from another pawn
				return Direction.DIRECTION_NONE;

			case PATH_BEHAVIOR_SEAK_PAWN:		// Agent moves directly onto another pawn
				return Direction.DIRECTION_NONE;

			case PATH_BEHAVIOR_ROUTE_TO_LOCATION: // Agent follows a path to a specific location
				if (CurrentPathWalker != null) {
					return CurrentPathWalker.nextDirection();
				} else {
					try {
						if (PathFuture.isDone()) {
							CurrentPath = (MapPath) PathFuture.get();
							CurrentPathWalker = CurrentPath.getPathWalker();
							return CurrentPathWalker.nextDirection();
						} else if (PathFuture.isCancelled()) {
							//Set future to null. Maybe we succeed next time...
							PathFuture = null;
						}
					} catch (Exception e) {
						//Exceptions.printStackTrace(e);
					}
					return Direction.DIRECTION_NONE;
				}

			case PATH_BEHAVIOR_ROUTE_TO_AREA:	  // Agent follows a path to the closest location in an area
				return Direction.DIRECTION_NONE;

			case PATH_BEHAVIOR_TRAPPED:		// Agent is Trapped unable to reach a desired area
				return Direction.DIRECTION_NONE;
		}
		return Direction.DIRECTION_NONE;
	}

	public void lastStepInvalid() {	 // Tell the Controller the last step order was invalid
	}

	public void setBehaviorMode(MovementBehavior NewBehavior) {
		if (CurrentMovementBehavior != NewBehavior)
			CurrentMovementBehavior = NewBehavior;
	}

	public boolean changeDestination(MapCoordinate NewDestination) {
		if (NewDestination != Destination) {
			Destination = NewDestination;

			PathFuture = ParentManager.findFuturePath(Modality, CurrentLocation, Destination);
			CurrentPathWalker = null;
			CurrentPath = null;
			return true;
		}
		return false;
	}

	public MapCoordinate getDestination() {
		return Destination;
	}

	public MovementModality getMovementModality() {
		return Modality;
	}

	public int getModalityIndex() {
		return ModalityGridIndex;
	}

	public void setLocation(MapCoordinate NewLocation) {
		CurrentLocation = NewLocation;
	}

	public float getPathEstimate(MapCoordinate TestDestination) {
		return 0;
	}

	public boolean isDestinationReachable(MapCoordinate TestDestination) {
		return ParentManager.isPathPossible(Modality, CurrentLocation, TestDestination);
	}

	public void wipePath() {
		Destination = null;
		CurrentPath = null;
		CurrentPathWalker = null;
	}
}
