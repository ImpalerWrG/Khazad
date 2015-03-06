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

import static Job.TaskType.TASK_DIG;
import static Job.TaskType.TASK_DROP_OFF;
import static Job.TaskType.TASK_GOTO;
import static Job.TaskType.TASK_HAUL;
import static Job.TaskType.TASK_IDLE;
import static Job.TaskType.TASK_PICK_UP;
import static Job.TaskType.TASK_SLEEP;
import Map.GameMap;
import Map.MapCoordinate;
import Map.Direction;
import Map.CubeShape;
import Game.Pawn;
import PathFinding.Navigator;

/**
 * Smallest possible unit of work or activity by a Pawn, Jobs are broken down
 * into a series of individual tasks that can be assigned to Citizens or invaders
 * @author Impaler
 */
public class Task {
		
	public final Job ParentJob;
	public final TaskType type;
	public final MapCoordinate worklocation;
	public final Direction workdirection;
	
	public boolean Completed;
	public boolean Begun;

	public Task(Job Parent, TaskType NewType, MapCoordinate location) {
		ParentJob = Parent;
		type = NewType;
		worklocation = location;
		workdirection = null;
		Begun = false;
		Completed = false;
	}
	
	public long Begin(Pawn Host) {
		Direction MovementDirection;
		switch (type) {
				
			case TASK_IDLE:
				break;
			case TASK_SLEEP:
				break;
			case TASK_PICK_UP:
				break;
			case TASK_HAUL:
				break;
			case TASK_GOTO:
				Host.getNavigator().setBehaviorMode(Navigator.MovementBehavior.PATH_BEHAVIOR_ROUTE_TO_LOCATION);
				Host.getNavigator().ChangeDestination(worklocation);
				MovementDirection = Host.getNavigator().getNextStep();
				Host.setMovementDiretion(MovementDirection);
				Begun = true;

				return Host.AttemptMove(MovementDirection);
				
			case TASK_DROP_OFF:
				break;
			case TASK_DIG:
				Begun = true;	
				return 100;  // Base on material hardness
				
			case TASK_LOITER:
				Host.getNavigator().setBehaviorMode(Navigator.MovementBehavior.PATH_BEHAVIOR_WANDER_AIMLESSLY);
				MovementDirection = Host.getNavigator().getNextStep();
				Host.setMovementDiretion(MovementDirection);
				Begun = true;
				
				return Host.AttemptMove(MovementDirection);
		}
		return 1;
	}

	public long Continue(Pawn Host) {
		switch (type) {

			case TASK_IDLE:
				break;
			case TASK_SLEEP:
				break;
			case TASK_PICK_UP:
				break;
			case TASK_HAUL:
				break;
			case TASK_GOTO:
				return Host.UpdatePosition();

			case TASK_DROP_OFF:
				break;
			case TASK_DIG:
				ExcavateJob Excavation = (ExcavateJob) ParentJob;
				CubeShape DesignatedShape = Excavation.getDesignation(worklocation);
				GameMap.getMap().ExcavateCube(worklocation, DesignatedShape.clone());

				if (DesignatedShape.ExcavationEquivilent(GameMap.getMap().getCubeShape(worklocation))) {
					Excavation.CompleteDesignation(worklocation);
					Completed = true;
				} else {
					return 100;  // Base on material hardness
				}
				break;
			case TASK_LOITER:
				Completed = true;
				return Host.UpdatePosition();
		}
		return 1;
	}
	
	public void Finalize(long CurrentTick, Pawn Host) {
		switch (type) {

			case TASK_IDLE:
				break;
			case TASK_SLEEP:
				break;
			case TASK_PICK_UP:
				break;
			case TASK_HAUL:
				break;
			case TASK_GOTO:
				break;

			case TASK_DROP_OFF:
				break;
			case TASK_DIG:
				//GameMap.getMap().UpdateCubeShape(worklocation, new CubeShape());
				break;
		}
	}
	
	public void Interupt(long CurrentTick, Pawn Host) {

	}
	
	String getName() {
		return "A simple task";
	}
}
