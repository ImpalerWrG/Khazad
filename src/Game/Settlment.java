/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Game;

import java.util.ArrayList;

/**
 * Object representing the players settlment, holds all Citizens, history and
 * work tasks being ordered by the player
 * @author Impaler
 */
public class Settlment {
	
	ArrayList<Citizen> Citizens;
	JobManager JobSystem;
	
	public Settlment() {
		Citizens = new ArrayList<Citizen>();
		JobSystem = new JobManager();
	}
	
	public void addCitizen() {
		
	}
	
	
}
