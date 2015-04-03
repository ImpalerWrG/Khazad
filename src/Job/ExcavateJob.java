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

import java.util.HashMap;
import java.util.BitSet;

import Game.Pawn;
import Map.GameMap;
import Map.CellCoordinate;
import Map.MapCoordinate;
import Map.CubeShape;
import Map.Zone;
import Interface.VolumeSelection;
import Map.Direction;
import PathFinding.MovementModality;
import PathFinding.PathFinding;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.Serializable;
import java.util.Map;

/**
 *
 * @author Impaler
 */
public class ExcavateJob extends Job implements Serializable {
	private static final long serialVersionUID = 1;

	HashMap<CellCoordinate, CubeShape[]> Designations;
	HashMap<CellCoordinate, BitSet> AccessibleExcavations;  // Excavation targets that are accesible
	HashMap<CellCoordinate, BitSet> AssignedExcavations;

	int DesignationCount = 0;
	int AccessibleExcavationCount = 0;
	int AssignedExcavationsCount = 0;

	HashMap<MapCoordinate, MapCoordinate> AccessibleMap; // The locations from which an excatation target is accesible
	HashMap<Pawn, MapCoordinate> AssignedWorkers;

	Zone ExcavationZone;
	GameMap map = null;
	transient PathFinding paths;
	MovementModality Modality;

	public ExcavateJob(GameMap map) {
		Designations = new HashMap<CellCoordinate, CubeShape[]>();
		AccessibleExcavations = new HashMap<CellCoordinate, BitSet>();
		AssignedExcavations = new HashMap<CellCoordinate, BitSet>();

		Name = "Delving Deeply";
		AccessibleMap = new HashMap<MapCoordinate, MapCoordinate>();
		AssignedWorkers = new HashMap<Pawn, MapCoordinate>();

		Modality = new MovementModality(MovementModality.MovementType.MOVEMENT_TYPE_WALK, 1, 1);
		this.map = map;
		Priority = 10;
		paths = PathFinding.getSingleton();
	}
	
	// this method is used by serialization
	private void readObject(ObjectInputStream ois) throws ClassNotFoundException, IOException {
		// default deserialization
		ois.defaultReadObject();
		// fix transients
		paths = PathFinding.getSingleton();
	}

	public void addDesignations(VolumeSelection Selection, Zone SourceZone, CubeShape NewShape) {
		this.ExcavationZone = SourceZone;
		MapCoordinate Origin = Selection.OriginLocation;
		MapCoordinate Terminal = Selection.TerminalLocation;

		for (int x = Origin.X; x < Terminal.X + 1; x++) {
			for (int y = Origin.Y; y < Terminal.Y + 1; y++) {
				for (int z = Origin.Z; z < Terminal.Z + 1; z++) {
					MapCoordinate TargetCoords = new MapCoordinate(x, y, z);
					CellCoordinate CellCoords = new CellCoordinate(TargetCoords);
					int CubeIndex = TargetCoords.CubeIntIndex();

					CubeShape[] DesignationShapes = Designations.get(CellCoords);
					BitSet AccessibleLocation = AccessibleExcavations.get(CellCoords);
					BitSet AssignedLocations = AssignedExcavations.get(CellCoords);

					if (map.getCubeShape(TargetCoords) != NewShape) {
						if (DesignationShapes == null) {
							DesignationShapes = new CubeShape[MapCoordinate.CUBESPERCELL];
							Designations.put(CellCoords, DesignationShapes);
						}
						DesignationShapes[CubeIndex] = NewShape;
						DesignationCount ++;

						if (AccessibleLocation == null) {
							AccessibleLocation = new BitSet(MapCoordinate.CUBESPERCELL);
							AccessibleExcavations.put(CellCoords, AccessibleLocation);
						}
						if (AssignedLocations == null) {
							AssignedLocations = new BitSet(MapCoordinate.CUBESPERCELL);
							AssignedExcavations.put(CellCoords, AssignedLocations);
						}

						CubeShape CurrentShape = GameMap.getMap().getCubeShape(TargetCoords);

						if (!CurrentShape.isSolid()) {
							BitSet DirectionFlags = paths.getDirectionFlags(TargetCoords, Modality);
							if (DirectionFlags.cardinality() != 0) {
								AccessibleMap.put(TargetCoords.clone(), TargetCoords.clone());
								AccessibleExcavationCount++;
								//break;
							}
						}

						// Test for Accesability of this Coordinate
						BitSet DirectionFlags;
						for (Direction dir: Direction.AXIAL_DIRECTIONS) {
							MapCoordinate AdjacentcCoords = TargetCoords.clone();
							AdjacentcCoords.TranslateMapCoordinates(dir);
							CubeShape AdjacentShape = GameMap.getMap().getCubeShape(AdjacentcCoords);

							if (AdjacentShape.isEmpty() || (dir == Direction.DIRECTION_DOWN && !AdjacentShape.isSolid())) {
								DirectionFlags = paths.getDirectionFlags(AdjacentcCoords, Modality);
								if (DirectionFlags.cardinality() != 0) {
									AccessibleLocation.set(CubeIndex);
									AccessibleMap.put(TargetCoords.clone(), AdjacentcCoords.clone());
									AccessibleExcavationCount++;
									break;
								}
							}
						}

						AccessibleLocation.set(CubeIndex, false);
						AssignedLocations.set(CubeIndex, false);
					}
				}
			}
		}
	}

	public CubeShape getDesignation(MapCoordinate Coords) {
		CellCoordinate CellCoords = new CellCoordinate(Coords);
		CubeShape[] DesignationShapes = Designations.get(CellCoords);
		return (DesignationShapes != null) ? DesignationShapes[Coords.CubeIntIndex()] : null;
	}

	public void CompleteDesignation(MapCoordinate Coords) {
		AccessibleMap.remove(Coords);
		ExcavationZone.removeMapCoordinate(Coords);

		DesignationCount--;
		AccessibleExcavationCount--;
		AssignedExcavationsCount--;

		// test adjacent for new accesability
		if (paths.getDirectionFlags(Coords, Modality).cardinality() > 0) {
			BitSet DirectionFlags;
			for (Direction dir: Direction.AXIAL_DIRECTIONS) {
				MapCoordinate AdjacentcCoords = Coords.clone();
				AdjacentcCoords.TranslateMapCoordinates(dir);
				CubeShape AdjacentShape = GameMap.getMap().getCubeShape(AdjacentcCoords);
				CubeShape DesignationShape = getDesignation(AdjacentcCoords);

				if (DesignationShape != null && !DesignationShape.ExcavationEquivilent(AdjacentShape)) {
					BitSet AccessibleLocation = AccessibleExcavations.get(new CellCoordinate(Coords));
					AccessibleLocation.set(Coords.CubeIntIndex());
					AccessibleMap.put(AdjacentcCoords.clone(), Coords.clone());
					AccessibleExcavationCount++;
				}
			}
		}

		if (DesignationCount == 0) {
			Manager.JobCompleted(this);
		} else {
			if (AccessibleExcavationCount == 0 && AssignedExcavationsCount == 0) {
				this.Paused = true;
			}
		}
	}

	public boolean isAssigned(MapCoordinate Coords) {
		CellCoordinate CellCoords = new CellCoordinate(Coords);
		BitSet Assignments = AssignedExcavations.get(CellCoords);
		return Assignments.get(Coords.CubeIntIndex());		
	}

	public Task nextTask(Pawn IdlePawn) {
		Task OldTask = IdlePawn.getTask();
		if (OldTask.ParentJob == this) {
			if (OldTask.type == TaskType.TASK_GOTO) {
				MapCoordinate TargetExcavation = AssignedWorkers.get(IdlePawn);

				Task newTask = new Task(this, TaskType.TASK_DIG, TargetExcavation);
				return newTask;
			}
		}

		MapCoordinate ExcavateCube = null;
		MapCoordinate AccsibleCube = null;

		for (Map.Entry<MapCoordinate, MapCoordinate> entry : AccessibleMap.entrySet()) {
			// distance test for best location
			if (!isAssigned(entry.getKey())) {
				AccsibleCube = entry.getValue();
				ExcavateCube = entry.getKey();
				break;
			}
		}

		if (ExcavateCube != null) {
			AssignedWorkers.put(IdlePawn, ExcavateCube);
			CellCoordinate CellCoords = new CellCoordinate(ExcavateCube);
			BitSet Assignments = AssignedExcavations.get(CellCoords);
			Assignments.set(ExcavateCube.CubeIntIndex());
			AssignedExcavationsCount++;

			Task newTask = new Task(this, TaskType.TASK_GOTO, AccsibleCube);
			return newTask;
		}

		if (!needsWorkers()) {
			return Manager.IdleCitizen(IdlePawn);
		} else {
			IdlePawn.onBreak = true;
			return IdlePawn.FindTask();
		}
	}

	public boolean needsWorkers() {
		return (AccessibleExcavationCount > AssignedExcavationsCount) ? true : false;
	}

	public float EvaluatePawn(Pawn CandidateCitizen) {
		float Evaluation = 0;
		if (Workers.contains(CandidateCitizen))
			Evaluation += 1;

		// distance test
		return Evaluation + 2;
	}

	public void JobCompleted() {

	}
	
	@Override
	public String getName() {
		return "Excavating";
	}
}