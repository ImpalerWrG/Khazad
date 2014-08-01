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
along with Khazad.  If not, see <http://www.gnu.org/licenses/> */

package Game;

import Job.Job;
import Job.Task;
import Core.Dice;

import Map.Direction;
import Map.GameMap;
import Map.MapCoordinate;
import Map.CubeShape;

import PathFinding.MovementModality;
import PathFinding.Navigator;
import PathFinding.PathFinding;

/**
 * A moving creature, derived from Actor but with the addition of a Navigator object 
 * linking it to the Pathfinding engine and a set of RPG style attributes.
 * 
 * @author Impaler
 */
public class Pawn extends Actor {

	boolean Moving;
	Direction CurrentMovementDirection;

	long ActionDuration = 1;
	long ActionStarted;

	Navigator PathNavigator;
	public Task CurrentTask;
	public Job CurrentJob;

	Dice AttributeDice;

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

		PathNavigator = new Navigator(SpawnLocation, new MovementModality(MovementModality.MovementType.WALK_MOVEMENT, 1, 1));
		PathNavigator.setBehaviorMode(Navigator.MovementBehavior.PATH_BEHAVIOR_ROUTE_TO_LOCATION);

		CurrentMovementDirection = Direction.DIRECTION_NONE;

		AttributeDice = new Dice();
		AttributeDice.Seed(id);
		Speed = 5;
		//Speed = (byte) (AttributeDice.Roll(1, 4) + AttributeDice.Roll(1, 4));  // 5 is normal dwarven speed
	}
	
	public Navigator getNavigator() {
		return PathNavigator;
	}

	public long AttemptMove(Direction MovementDirection) {
		float EdgeCost = PathFinding.getSinglton().getEdgeCost(LocationCoordinates, MovementDirection, PathNavigator.getMovementModality());
		
		if (EdgeCost != -1) {
			return (int) (EdgeCost / (Speed / 5) * 12);
		} else {
			return 1;  // signal falure to job manager?
		}
	}

	public long UpdatePosition() {
		setLocation(new MapCoordinate(LocationCoordinates, CurrentMovementDirection));
		CurrentMovementDirection = PathNavigator.getNextStep();

		if (CurrentMovementDirection == Direction.DIRECTION_DESTINATION) {
			CurrentTask.Completed = true;
			return 1;
		} else {
			return AttemptMove(CurrentMovementDirection);
		}
	}

	public boolean isDestinationReachable(MapCoordinate Destination) {
		return PathNavigator.isDestinationReachable(Destination);
	}

	@Override
	public final void setLocation(MapCoordinate NewLocation) {
		super.setLocation(NewLocation);
		PathNavigator.setLocation(NewLocation);
	}

	public void setTask(Task NewTask) {
		if (CurrentTask != NewTask) {
			CurrentTask = NewTask;
		}
	}

	public float getActionFraction(long CurrentTick) {
		return (CurrentTick - ActionStarted) / ((float) ActionDuration);
	}

	public void setMovementDiretion(Direction newDirection) {
		CurrentMovementDirection = newDirection;
	}

	public Direction getMovementDirection() {
		return CurrentMovementDirection;
	}

	public MovementModality getMovementModality() {
		return PathNavigator.getMovementModality();
	}

	@Override
	long Wake(long CurrentTick) {
		//super.Wake(CurrentTick);

		if (CurrentTask.Completed) {
			CurrentTask.Finalize(CurrentTick, this);
			setTask(CurrentTask.ParentJob.nextTask(this));
		}

		if (!CurrentTask.Begun) {
			ActionDuration = CurrentTask.Begin(this);
		} else {
			ActionDuration = CurrentTask.Continue(this);
		}
		ActionStarted = CurrentTick;
		WakeTick = CurrentTick + ActionDuration;

		return WakeTick;
	}
}
