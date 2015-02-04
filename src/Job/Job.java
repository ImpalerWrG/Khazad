/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Job;

import Game.Pawn;
import java.util.ArrayList;
import java.util.concurrent.ConcurrentLinkedQueue;

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

	public enum JobClass {
		JOB_REAL,  // Jobs ordered by the player that compete for workers
		JOB_BREAK,  // Short Jobs to fill worker needs
		JOB_IDLE  // Jobs originating from the workers themselves
	}

	JobType Type;
	JobClass Class;

	boolean Paused;
	byte Priority;

	String Name;
	ConcurrentLinkedQueue<Pawn> Workers;
	int WorkersOnBrek;
	JobManager Manager;
	
	public Job() {
		Workers = new ConcurrentLinkedQueue<Pawn>();
	}
	
	public boolean addPawn(Pawn NewPawn) {
		if (Workers.contains(NewPawn))
			return false;
		
		if (NewPawn.PrimaryJob != null) {
			NewPawn.PrimaryJob.releaseCitizen(NewPawn);
		}
		Workers.add(NewPawn);
		NewPawn.PrimaryJob = this;
		Task newTask = nextTask(NewPawn);
		NewPawn.setTask(newTask);
		return true;
	}

	public void releaseCitizen(Pawn OldCitizen) {
		if (OldCitizen.PrimaryJob == this)
			OldCitizen.PrimaryJob = null;
		if (Workers.contains(OldCitizen)) {
			Workers.remove(OldCitizen);
			OldCitizen.PrimaryJob = null;
			//OldCitizen.CurrentTask = null;
		}
	}

	public void setOnBreak(Pawn OnBreakPawn) {
		OnBreakPawn.onBreak = true;
		WorkersOnBrek++;
	}

	public abstract boolean needsWorkers();

	public abstract Task nextTask(Pawn IdleCitizen);
	
	public abstract float EvaluatePawn(Pawn IdleCitizen);
}
