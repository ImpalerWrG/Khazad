/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Game;

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

	Task CurrentTask;
	Job CurrentJob;
	
	public Citizen(int id, MapCoordinate SpawnLocation) {
		super(id, SpawnLocation);
		CurrentTask = new Task(Task.TaskType.TASK_IDLE, SpawnLocation);
	}
	
	public void setTask(Task NewTask) {
		if (CurrentTask != NewTask) {
			CurrentTask = NewTask;
			// change behavior to achive task
		}
	}
}
