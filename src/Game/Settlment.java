/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Game;

import Job.JobManager;
import java.util.ArrayList;

/**
 * Object representing the players settlment, holds all Citizens, history and
 * work tasks being ordered by the player
 * @author Impaler
 */
public class Settlment {

	// Settlment data, wealth, honor, diplomatic status etc

	private ArrayList<Citizen> Citizens;
	JobManager JobSystem;

	public Settlment() {
		Citizens = new ArrayList<Citizen>();
		JobSystem = new JobManager();
	}

	public void addCitizen(Citizen NewCitizen) {
		Citizens.add(NewCitizen);
		JobSystem.addCitizen(NewCitizen);
	}
	
	public JobManager getJobManager() {
		return JobSystem;
	}
}
