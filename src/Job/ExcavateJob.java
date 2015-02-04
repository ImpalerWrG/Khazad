/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Job;

import java.util.HashMap;
import java.util.BitSet;
import java.util.ArrayList;
import java.util.PriorityQueue;

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
import java.util.Map;

/**
 *
 * @author Impaler
 */
public class ExcavateJob extends Job {

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
	PathFinding paths = PathFinding.getSinglton();
			
	public ExcavateJob(GameMap map) {
		Designations = new HashMap<CellCoordinate, CubeShape[]>();
		AccessibleExcavations = new HashMap<CellCoordinate, BitSet>();
		AssignedExcavations = new HashMap<CellCoordinate, BitSet>();
		
		Name = "Delving Deeply";
		//AccesibleQueue = new PriorityQueue<MapCoordinate>();
		//AccesibleList = new	ArrayList<MapCoordinate>();
		AccessibleMap = new HashMap<MapCoordinate, MapCoordinate>();
		AssignedWorkers = new HashMap<Pawn, MapCoordinate>();

		this.map = map;
		Priority = 10;
	}

	public void addDesignations(VolumeSelection Selection, Zone SourceZone, CubeShape NewShape) {
		this.ExcavationZone = SourceZone;
		MapCoordinate Origin = Selection.OriginLocation;
		MapCoordinate Terminal = Selection.TerminalLocation;

		for (int x = Origin.X; x < Terminal.X + 1; x++) {
			for (int y = Origin.Y; y < Terminal.Y + 1; y++) {
				for (int z = Origin.Z; z < Terminal.Z + 1; z++) {
					MapCoordinate TargetCube = new MapCoordinate(x, y, z);
					CellCoordinate CellCoords = new CellCoordinate(TargetCube);
					int CubeIndex = TargetCube.CubeInt();

					CubeShape[] DesignationShapes = Designations.get(CellCoords);
					BitSet AccessibleLocation = AccessibleExcavations.get(CellCoords);
					BitSet AssignedLocations = AssignedExcavations.get(CellCoords);

					if (map.getCubeShape(TargetCube) != NewShape) {
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

						// Test for Accesability of this Coordinate
						BitSet DirectionFlags;
						for (Direction dir: Direction.AXIAL_DIRECTIONS) {
							MapCoordinate AdjacentcCoords = TargetCube.clone();
							AdjacentcCoords.TranslateMapCoordinates(dir);
							DirectionFlags = paths.getDirectionFlags(AdjacentcCoords, new MovementModality(MovementModality.MovementType.WALK_MOVEMENT, 1, 1));
							if (DirectionFlags.cardinality() != 0) {
								AccessibleLocation.set(CubeIndex);
								AccessibleExcavationCount++;

								//AccesibleList.add(TargetCube.clone());
								AccessibleMap.put(TargetCube.clone(), AdjacentcCoords.clone());
								break;
							}
						}

						if (AssignedLocations == null) {
							AssignedLocations = new BitSet(MapCoordinate.CUBESPERCELL);
							AssignedExcavations.put(CellCoords, AssignedLocations);
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
		return DesignationShapes[Coords.CubeInt()];
	}

	public void CompleteDesignation(MapCoordinate Coords) {
		AccessibleMap.remove(Coords);
		ExcavationZone.removeMapCoordinate(Coords);
		
		DesignationCount--;
		AccessibleExcavationCount--;
		AssignedExcavationsCount--;
		
		if (DesignationCount == 0)
			Manager.JobCompleted(this);
	}

	public boolean isAssigned(MapCoordinate Coords) {
		CellCoordinate CellCoords = new CellCoordinate(Coords);
		BitSet Assignments = AssignedExcavations.get(CellCoords);
		return Assignments.get(Coords.CubeInt());		
	}

	/*
	@Override
	// add worker to job, if it was not already on this job return true
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
		Task OldTask = IdlePawn.CurrentTask;
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
			}
		}

		if (ExcavateCube != null) {
			AssignedWorkers.put(IdlePawn, ExcavateCube);
			CellCoordinate CellCoords = new CellCoordinate(ExcavateCube);
			BitSet Assignments = AssignedExcavations.get(CellCoords);
			Assignments.set(ExcavateCube.CubeInt());
			AssignedExcavationsCount++;

			Task newTask = new Task(this, TaskType.TASK_GOTO, AccsibleCube);
			return newTask;
		}

		if (DesignationCount < Workers.size()) {
			releaseCitizen(IdlePawn);
			return Manager.IdleCitizen(IdlePawn);
		} else {
			IdlePawn.onBreak = true;
			// Pawn finds job
			return null;
		}
	}

	public boolean needsWorkers() {
		if (DesignationCount > Workers.size())
			return true;
		return false;
	}

	public float EvaluatePawn(Pawn CandidateCitizen) {
		float Evaluation = 0;
		if (Workers.contains(CandidateCitizen))
			Evaluation += 1;
		return Evaluation + 2;
	}

	public void JobCompleted() {
		
	}
}