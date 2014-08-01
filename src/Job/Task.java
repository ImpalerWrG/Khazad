/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Job;

import static Job.TaskType.TASK_DIG;
import static Job.TaskType.TASK_DROP_OFF;
import static Job.TaskType.TASK_GOTO;
import static Job.TaskType.TASK_HAUL;
import static Job.TaskType.TASK_IDLE;
import static Job.TaskType.TASK_PICK_UP;
import static Job.TaskType.TASK_SLEEP;
import Map.GameMap;
import Map.MapCoordinate;
import Map.Direction;
import Map.CubeShape;
import Game.Pawn;

/**
 * Smallest possible unit of work or activity by a Pawn, Jobs are broken down
 * into a series of individual tasks that can be assigned to Citizens or invaders
 * @author Impaler
 */
public class Task {
		
	public final Job ParentJob;
	public final TaskType type;
	public final MapCoordinate worklocation;
	public final Direction workdirection;
	
	public boolean Completed;
	public boolean Begun;

	public Task(Job Parent, TaskType NewType, MapCoordinate location) {
		ParentJob = Parent;
		type = NewType;
		worklocation = location;
		workdirection = null;
		Begun = false;
		Completed = false;
	}
	
	public long Begin(Pawn Host) {
		switch (type) {

			case TASK_IDLE:
				break;
			case TASK_SLEEP:
				break;
			case TASK_PICK_UP:
				break;
			case TASK_HAUL:
				break;
			case TASK_GOTO:
				Host.getNavigator().ChangeDestination(worklocation);
				Direction MovementDirection = Host.getNavigator().getNextStep();
				Host.setMovementDiretion(MovementDirection);
				Begun = true;

				return Host.AttemptMove(MovementDirection);
				
			case TASK_DROP_OFF:
				break;
			case TASK_DIG:
				Begun = true;	
				return 100;  // Base on material hardness
		}
		return 1;
	}

	public long Continue(Pawn Host) {
		switch (type) {

			case TASK_IDLE:
				break;
			case TASK_SLEEP:
				break;
			case TASK_PICK_UP:
				break;
			case TASK_HAUL:
				break;
			case TASK_GOTO:
				return Host.UpdatePosition();

			case TASK_DROP_OFF:
				break;
			case TASK_DIG:
				GameMap.getMap().UpdateCubeShape(worklocation, new CubeShape((byte) 0));

				if (GameMap.getMap().getCubeShape(worklocation).isEmpty()) {
					Completed = true;
				} else {
					return 100;  // Base on material hardness
				}
		}
		return 1;
	}
	
	public void Finalize(long CurrentTick, Pawn Host) {
		switch (type) {

			case TASK_IDLE:
				break;
			case TASK_SLEEP:
				break;
			case TASK_PICK_UP:
				break;
			case TASK_HAUL:
				break;
			case TASK_GOTO:
				break;

			case TASK_DROP_OFF:
				break;
			case TASK_DIG:
				//GameMap.getMap().UpdateCubeShape(worklocation, new CubeShape());
				break;
		}
	}
	
	public void Interupt(long CurrentTick, Pawn Host) {

	}
	
	String getName() {
		return "A simple task";
	}
}
