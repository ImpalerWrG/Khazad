/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Game;

import Job.*;
import Map.MapCoordinate;

/**
 *
 * @author Impaler
 */
public class Citizen extends Pawn {
	
	// Family/clan relationships
	
	// Happyness/ moods/ hunger/ thirst/ wealth/ honor / other mutable status
	
	// personality/ tastes/ other fixed status

	// Proffessions / levels/ skills

	//Job CurrentJob;
	
	public Citizen(int id, MapCoordinate SpawnLocation) {
		super(id, SpawnLocation);
		CurrentTask = new Task(null, TaskType.TASK_IDLE, SpawnLocation);
	}
	
}
