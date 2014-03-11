/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Game;

import java.util.ArrayList;
import Map.Zone;

/**
 *
 * @author Impaler
 */
public class Job {
	
	public enum JobType {
		JOB_IDLE,
		JOB_DIG,
		JOB_HAUL,
		JOB_WANDER  // temporary testing
	}

	JobType Type;
	boolean Active;
	byte Priority;
	Zone WorkPlace;

	String Name;
	ArrayList<Citizen> Workers;
	
	public Job(JobType NewType) {
		Type = NewType;
	}
	
	public Task nextTask(Citizen IdleCitizen) {
		// search for an appropriate task for this Citizen
		return null;
	}
}
