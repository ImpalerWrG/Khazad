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


import Map.GameMap;
import Map.Coordinates.MapCoordinate;
import Map.Coordinates.Direction;
import Map.BlockShape;
import Game.Pawn;
import PathFinding.Navigator;
import java.io.Serializable;

/**
 * Smallest possible unit of work or activity by a Pawn, Jobs are broken down
 * into a series of individual tasks that can be assigned to Citizens or invaders
 *
 * @author Impaler
 */
public class Task implements Serializable {

	public enum TaskType {

		TASK_IDLE,
		TASK_TRAPPED,
		TASK_SLEEP,
		TASK_PICK_UP,
		TASK_HAUL,
		TASK_GOTO,
		TASK_DROP_OFF,
		TASK_DIG,
		TASK_LOITER
	}

	private static final long serialVersionUID = 1;
	public final Job ParentJob;
	public final TaskType type;
	public final MapCoordinate worklocation;
	public BlockShape TestingBlockShape;
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

	public long beginTask(Pawn Host) {
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
				Host.getNavigator().changeDestination(worklocation);
				MovementDirection = Host.getNavigator().getNextStep();
				Host.setMovementDiretion(MovementDirection);
				Begun = true;

				return Host.attemptMove(MovementDirection);

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

				return Host.attemptMove(MovementDirection);
		}
		return 1;
	}

	public long continueTask(Pawn Host) {
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
				return Host.updatePosition();

			case TASK_DROP_OFF:
				break;
			case TASK_DIG:
				ExcavateJob Excavation = (ExcavateJob) ParentJob;
				BlockShape DesignatedShape = Excavation.getDesignation(worklocation);
				GameMap.getMap().excavateBlock(worklocation, DesignatedShape.clone());

				//Fall down to the new surface
				GameMap.getMap().getBlockShape(Host.getLocation(), TestingBlockShape);
				if (TestingBlockShape.isSky()) {
					MapCoordinate Newlocation = Host.getLocation().clone();
					Newlocation.translate(Direction.DIRECTION_DOWN);
					Host.setLocation(Newlocation);
				}

				GameMap.getMap().getBlockShape(worklocation, TestingBlockShape);
				if (DesignatedShape.isExcavationEquivilent(TestingBlockShape)) {
					Excavation.completeDesignation(worklocation);
					Completed = true;
				} else {
					return 1000;  // Base on material hardness
				}
				break;
			case TASK_LOITER:
				Completed = true;
				return Host.updatePosition();
		}
		return 1;
	}

	public void finalizeTask(long CurrentTick, Pawn Host) {
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
				//GameMap.getMap().UpdateBlockShape(worklocation, new BlockShape());
				break;
		}
	}

	public void interuptTask(long CurrentTick, Pawn Host) {
	}

	public String getName() {
		switch (type) {
			case TASK_IDLE:
				return "Idling";
			case TASK_SLEEP:
				return "Sleeping";
			case TASK_PICK_UP:
				return "Picking up";
			case TASK_HAUL:
				return "Hauling";
			case TASK_GOTO:
				return "Moving";
			case TASK_DROP_OFF:
				return "Dropping off";
			case TASK_DIG:
				return "Digging";
		}
		return "Unknown";
	}
}
