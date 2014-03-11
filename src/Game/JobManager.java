/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Game;

import Map.MapCoordinate;
import PathFinding.Navigator;
import PathFinding.PathManager;
import java.util.HashMap;
/**
 * Manager for prioritizing and alocating all Jobs and tasks inside a Settlment
 * @author Impaler
 */
public class JobManager {
	
	HashMap<String, Job> JobMap;
	
	public JobManager() {
		JobMap = new HashMap<String, Job>();
		
	}
	
	public void addJob(Job NewJob) {
		JobMap.put(NewJob.Name, NewJob);
	}
	
	public Task provideTask(Citizen IdleCitizen) {
		
		// examine Citizen skill set, position etc, generally keep
		// on them working on the next task of the same Job
		if (IdleCitizen.CurrentJob.Type == Job.JobType.JOB_WANDER) {
			MapCoordinate randomgoal;
			
		}
		
		return null;
	}
	
	/*
	public void RandomRoute() {
		PathManager Pathing = PathManager.getSinglton();
		if (Pathing.getDirectionFlags(LocationCoordinates).cardinality() != 0) {//Is the current location impassable
			MapCoordinate DestinationCoordinates = Pathing.Tester.getRandomPassableCoordinate();  // This needs to get DIFFERENT coords each time

			while (!PathNavigator.isDestinationReachable(DestinationCoordinates)) {
				DestinationCoordinates = Pathing.Tester.getRandomPassableCoordinate();
			}
			PathNavigator.ChangeDestination(DestinationCoordinates);
		} else {
			PathNavigator.setBehaviorMode(Navigator.MovementBehavior.PATH_BEHAVIOR_HALT);
			Moving = false;
		}
	}*/

}
