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

package Job;

import Game.Pawn;

import java.util.concurrent.ConcurrentLinkedQueue;

import java.io.Serializable;

/**
 *
 * @author Impaler
 */
public abstract class Job implements Serializable {

	private static final long serialVersionUID = 1;

	public enum JobType {

		JOB_IDLE,
		JOB_DIG,
		JOB_HAUL,
		JOB_WANDER  // temporary testing
	}

	public enum JobClass {

		JOB_REAL, // Jobs ordered by the player that compete for workers
		JOB_BREAK, // Short Jobs to fill worker needs
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

		Task newTask = nextTask(NewPawn);
		if (newTask != null) {
			if (NewPawn.PrimaryJob != null)
				NewPawn.PrimaryJob.releaseCitizen(NewPawn);

			Workers.add(NewPawn);
			NewPawn.PrimaryJob = this;
			NewPawn.setTask(newTask);
			return true;
		}
		return false;
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

	public abstract float evaluatePawn(Pawn IdleCitizen);
	
	public String getName() {
		return Name;
	}
}
