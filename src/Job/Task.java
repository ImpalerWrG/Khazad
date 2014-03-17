/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Job;

import Map.MapCoordinate;
import Map.Direction;

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

	public Task(Job Parent, TaskType NewType, MapCoordinate location) {
		ParentJob = Parent;
		type = NewType;
		worklocation = location;
		workdirection = null;
	}
	
	String getName() {
		return "A simple task";
	}
}
