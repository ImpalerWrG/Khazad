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

import Game.Citizen;
import Game.Pawn;

import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.ConcurrentHashMap;

/**
 * Manager for prioritizing and alocating all Jobs and tasks inside a Settlment
 * @author Impaler
 */
public class JobManager {

	private ConcurrentHashMap<String, Job> JobMap;
	private ConcurrentLinkedQueue<Pawn> IdleCitizens;

	boolean JobsDirty;

	public JobManager() {
		JobMap = new ConcurrentHashMap<String, Job>();
		IdleCitizens = new ConcurrentLinkedQueue<Pawn>();
		JobsDirty = false;
	}

	public void addCitizen(Citizen NewCitizen) {
		// Find the best Job for this Citizen and assign
		Job BestJob = FindBestJob(NewCitizen);
		if (BestJob != null) {
			BestJob.addPawn(NewCitizen);
		} else {
			IdleCitizen(NewCitizen);
		}
	}

	public Task IdleCitizen(Pawn TargetCitizen) {
		Job CurrentJob = TargetCitizen.PrimaryJob;
		if (CurrentJob != null)
			TargetCitizen.PrimaryJob.releaseCitizen(TargetCitizen);
		IdleCitizens.add(TargetCitizen);
		return TargetCitizen.FindTask();
	}

	public Job FindBestJob(Pawn TargetCitizen) {
		Job BestJob = null;
		float BestJobFitness  = 0;
		for (Job TargetJob: JobMap.values()) {
			if (!TargetJob.Paused && TargetJob.needsWorkers()) {
				float TempFitness = TargetJob.EvaluatePawn(TargetCitizen);
				if (TempFitness > BestJobFitness) {
					BestJob = TargetJob;
					BestJobFitness = TempFitness;
				}
			}
		}
		return BestJob;
	}

	public void addJob(Job NewJob) {
		JobMap.put(NewJob.Name, NewJob);
		NewJob.Manager = this;
		JobsDirty = true;
	}

	public void assignWorker() {
		
	}

	private void RebalanceJobs() {
		if (JobsDirty) {
			for (Job TargetJob : JobMap.values()) {
				int WorkersNeeded = TargetJob.Priority - TargetJob.Workers.size();
				if (WorkersNeeded > 0) {
					for (int i = 0; i < WorkersNeeded; i++) {
						if (!IdleCitizens.isEmpty()) {
							Pawn IdlePawn = IdleCitizens.poll();
							//IdleJob.releaseCitizen(IdlePawn);
							TargetJob.addPawn(IdlePawn);
						}
					}
				}
			}
		}
		JobsDirty = false;	
	}

	public void JobCompleted(Job CompletedJob) {
		for(Pawn Worker : CompletedJob.Workers) {
			//Worker.PrimaryJob = null;
			IdleCitizen(Worker);
		}
		JobMap.remove(CompletedJob);
	}

	public void update() {
		if (JobsDirty) {
			RebalanceJobs();
		}
		
		if (!IdleCitizens.isEmpty()) {
			for (Pawn citizen : IdleCitizens) {
				Job job = FindBestJob(citizen);
				if (job != null) {
					job.addPawn(IdleCitizens.poll());
				} else {
					// find Idle task
					//citizen.FindTask()
				}
			}
		}
	}
}
