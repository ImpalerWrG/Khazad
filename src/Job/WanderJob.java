/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Job;

import Game.Pawn;
import Map.MapCoordinate;
import PathFinding.PathFinding;


/**
 *
 * @author Impaler
 */
public class WanderJob extends Job {
	
	PathFinding Pathing;
	
	public WanderJob() {
		super();
		Name = "Wandering Aimlessly";

		Pathing = PathFinding.getSinglton();
		Type = Job.JobType.JOB_WANDER;
	}

	/*
	@Override
	public boolean addPawn(Pawn NewPawn) {
		if (super.addPawn(NewPawn)) {
			Task newTask = nextTask(NewPawn);
			NewPawn.setTask(newTask);
			return true;
		} else {
			return false;
		}
	}*/

	public Task nextTask(Pawn IdlePawn) {
		MapCoordinate Destination = RandomDestination(IdlePawn);
		if (Destination != null) {
			return new Task(this, TaskType.TASK_GOTO, Destination);
		} else {
			releaseCitizen(IdlePawn);
			return new Task(this, TaskType.TASK_TRAPPED, null);
		}
	}

	public boolean needsWorkers() {
		return true;
	}

	public float EvaluatePawn(Pawn CandidateCitizen) {
		float Evaluation = 0;
		if (Workers.contains(CandidateCitizen))
			Evaluation += 1;
		return Evaluation + 1;
	}

	public MapCoordinate RandomDestination(Pawn  Wanderer) {
		MapCoordinate StartLocation = Wanderer.getLocation();
		if (Pathing.getDirectionFlags(StartLocation, Wanderer.getMovementModality()).cardinality() != 0) {//Is the current location impassable
			MapCoordinate DestinationCoordinates = Pathing.Tester.getRandomPassableCoordinate();  // This needs to get DIFFERENT coords each time

			while (!Wanderer.isDestinationReachable(DestinationCoordinates)) {
				DestinationCoordinates = Pathing.Tester.getRandomPassableCoordinate();
			}
			return DestinationCoordinates;
		}
		return null;
	}
}
