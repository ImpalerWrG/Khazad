/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Job;

import Game.Citizen;
import Game.Pawn;

import java.util.ArrayList;
import java.util.HashMap;

/**
 * Manager for prioritizing and alocating all Jobs and tasks inside a Settlment
 * @author Impaler
 */
public class JobManager {

	HashMap<String, Job> JobMap;
	ArrayList<Pawn> IdleCitizens;
	//ArrayList<Citizen> WorkingCitizens;
	ArrayList<Job> NewJobs;

	public JobManager() {
		JobMap = new HashMap<String, Job>();
		IdleCitizens = new ArrayList<Pawn>();
		NewJobs = new ArrayList<Job>();
	}

	public void addCitizen(Citizen NewCitizen) {
		IdleCitizens.add(NewCitizen);
		
		// Find the best Job for this Citizen and assign
		Job BestJob = FindBestJob(NewCitizen);
		if (BestJob != null) 
			BestJob.addPawn(NewCitizen);
	}

	public Job FindBestJob(Pawn TargetCitizen) {
		Job BestJob = null;
		float BestJobFitness  = 0;
		for (Job TargetJob: JobMap.values()) {
			if (!TargetJob.Paused) {
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
		NewJobs.add(NewJob);
	}

	public void assignWorker() {
		
	}

	private void RebalanceWorkers() {
		for (Job job : NewJobs) {
			int WorkersNeeded = job.Priority - job.Workers.size();
			if (WorkersNeeded > 0) {
				for (int i = 0; i < WorkersNeeded; i++) {
					if (!IdleCitizens.isEmpty()) {
						int Index = IdleCitizens.size() - 1;
						job.addPawn(IdleCitizens.get(Index));
						IdleCitizens.remove(Index);				
					}
				}
			}
		}
		NewJobs.clear();
		
		for (Pawn citizen : IdleCitizens) {
			Job job = FindBestJob(citizen);
			job.addPawn(citizen);
			IdleCitizens.remove(citizen);
		}
	}

	public void update() {
		if (!NewJobs.isEmpty()) {
			
			RebalanceWorkers();
		}
	}
}
