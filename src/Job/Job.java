/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Job;

import Game.Pawn;
import java.util.ArrayList;

/**
 *
 * @author Impaler
 */
public abstract class Job {
	
	public enum JobType {
		JOB_IDLE,
		JOB_DIG,
		JOB_HAUL,
		JOB_WANDER  // temporary testing
	}

	JobType Type;
	boolean Paused;
	byte Priority;

	String Name;
	ArrayList<Pawn> Workers;
	
	public Job() {
		Workers = new ArrayList<Pawn>();
	}
	
	public void addCitizen(Pawn NewCitizen) {
		Workers.add(NewCitizen);
	}

	public void releaseCitizen(Pawn OldCitizen) {
		Workers.remove(OldCitizen);
	}

	public abstract Task nextTask(Pawn IdleCitizen);
	
	public abstract float EvaluatePawn(Pawn IdleCitizen);
}
