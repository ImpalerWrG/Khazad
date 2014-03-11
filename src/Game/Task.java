/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Game;

import Map.MapCoordinate;

/**
 * Smallest possible unit of work or activity by a Pawn, Jobs are broken down
 * into a series of individual tasks that can be assigned to Citizens or invaders
 * @author Impaler
 */
public class Task {
	
	public enum TaskType {
		TASK_IDLE,
		TASK_SLEEP,
		TASK_PICK_UP,
		TASK_HAUL,
		TASK_GOTO,
		TASK_DROP_OFF,
		TASK_DIG,
	}
	
	public final TaskType type;
	public final MapCoordinate worklocation;

	public Task(TaskType NewType, MapCoordinate location) {
		type = NewType;
		worklocation = location;
	}
}
