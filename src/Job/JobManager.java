/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Job;

import Game.Citizen;
import Map.MapCoordinate;

import java.util.ArrayList;
import java.util.HashMap;

/**
 * Manager for prioritizing and alocating all Jobs and tasks inside a Settlment
 * @author Impaler
 */
public class JobManager {

	HashMap<String, Job> JobMap;
	ArrayList<Citizen> IdleCitizens;
	//ArrayList<Citizen> WorkingCitizens;

	public JobManager() {
		JobMap = new HashMap<String, Job>();
		IdleCitizens = new ArrayList<Citizen>();
	}

	public void addCitizen(Citizen NewCitizen) {
		IdleCitizens.add(NewCitizen);
		
		// Find the best Job for this Citizen and assign
		Job BestJob = FindBestJob(NewCitizen);
		if (BestJob != null) 
			BestJob.addCitizen(NewCitizen);
	}

	public Job FindBestJob(Citizen TargetCitizen) {
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
	}

	public void assignWorker() {
		
	}
}
