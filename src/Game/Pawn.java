/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Game;

import Job.Task;
import Core.Dice;

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
	public Task CurrentTask;

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
		PathNavigator = new Navigator(SpawnLocation, 1, 1);
		PathNavigator.setBehaviorMode(Navigator.MovementBehavior.PATH_BEHAVIOR_ROUTE_TO_LOCATION);

		CurrentMovementDirection = Direction.DIRECTION_NONE;
		
		AttributeDice = new Dice();
		AttributeDice.Seed(id);
		Speed = 5;
		//Speed = (byte) (AttributeDice.Roll(1, 4) + AttributeDice.Roll(1, 4));  // 5 is normal dwarven speed
	}
	
	long AttemptMove(Direction MovementDirection) {
		float EdgeCost = PathManager.getSinglton().getEdgeCost(LocationCoordinates, MovementDirection);
		
		if (EdgeCost != -1) {
			Moving = true;
			MovementDuration = (int) (EdgeCost / (Speed / 5) * 12);
			return MovementDuration;
		} else {
			return 1;  // signal falure to job manager?
		}
	}

	long UpdatePosition(long CurrentTick) {

		if (Moving) {
			if (CurrentTick >= (MovementStarted + MovementDuration)) { // Done
				MapCoordinate NewLocation = new MapCoordinate(LocationCoordinates, CurrentMovementDirection);
				setLocation(NewLocation);
				Moving = false;
			}
			return 1;
		} else {
			CurrentMovementDirection = PathNavigator.getNextStep();
			if (CurrentMovementDirection != Direction.DIRECTION_NONE) {
				if (CurrentMovementDirection != Direction.DIRECTION_DESTINATION) {
					MovementStarted = CurrentTick;
					return AttemptMove(CurrentMovementDirection);
				} else {
					CurrentTask = CurrentTask.ParentJob.nextTask(this);
					PathNavigator.ChangeDestination(CurrentTask.worklocation);
				}
			}
			return 1;
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
			
			switch (CurrentTask.type) {

				case TASK_IDLE:
					break;
				case TASK_SLEEP:
					break;
				case TASK_PICK_UP:
					break;
				case TASK_HAUL:
					break;
				case TASK_GOTO:
					PathNavigator.ChangeDestination(CurrentTask.worklocation);
					break;
					
				case TASK_DROP_OFF:
					break;
				case TASK_DIG:
					break;
			}

			// change behavior to achive task ?
		}
	}

	public float getMovementFraction(long CurrentTick) {
		return (CurrentTick - MovementStarted) / ((float) MovementDuration);
	}
	
	public Direction getMovementDirection() {
		return CurrentMovementDirection;
	}
	
	@Override
	long Wake(long CurrentTick) {
		super.Wake(CurrentTick);
		
		if (CurrentTick >= WakeTick) {
					
			switch (CurrentTask.type) {

				case TASK_IDLE:
					break;
				case TASK_SLEEP:
					break;
				case TASK_PICK_UP:
					break;
				case TASK_HAUL:
					break;
				case TASK_GOTO:
					WakeTick = CurrentTick + UpdatePosition(CurrentTick);
					break;
					
				case TASK_DROP_OFF:
					break;
				case TASK_DIG:
					break;
			}
		}
		return WakeTick;
	}
}
