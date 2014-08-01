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
		Type = Job.JobType.JOB_WANDER;
		Pathing = PathFinding.getSinglton();
	}

	@Override
	public void addPawn(Pawn NewCitizen) {
		super.addPawn(NewCitizen);
		Task Wandering = nextTask(NewCitizen);
		NewCitizen.setTask(Wandering);
	}

	@Override
	public void releaseCitizen(Pawn OldCitizen) {
		super.releaseCitizen(OldCitizen);
	}

	public Task nextTask(Pawn IdlePawn) {
		MapCoordinate Destination = RandomDestination(IdlePawn);
		if (Destination != null) {
			return new Task(this, TaskType.TASK_GOTO, Destination);
		} else {
			releaseCitizen(IdlePawn);
			return new Task(this, TaskType.TASK_TRAPPED, null);
		}
	}

	public float EvaluatePawn(Pawn IdleCitizen) {
		return 1.0f;
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
