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
import Map.Coordinates.ChunkCoordinate;
import Map.Coordinates.MapCoordinate;
import Map.Coordinates.BlockCoordinate;
import Map.BlockShape;
import Map.Zone;
import Interface.VolumeSelection;
import Map.Coordinates.Direction;
import PathFinding.MovementModality;
import PathFinding.PathManager;
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
	// The desired final shapes of Blocks
	HashMap<ChunkCoordinate, BlockShape[]> Designations;
	// Boolens for Excavation targets that are accesible and assigned respectivly
	HashMap<ChunkCoordinate, BitSet> AccessibleExcavations;
	HashMap<ChunkCoordinate, BitSet> AssignedExcavations;
	
	BlockShape CurrentBlockShape, AdjacentBlockShape;
	// Counts on each stage of work
	int DesignationCount = 0;
	int AccessibleExcavationCount = 0;
	int AssignedExcavationsCount = 0;
	// The locations from which an excatation target is accesible;
	HashMap<MapCoordinate, MapCoordinate> AccessibleMap;
	// Which Pawn is assigned to excavate each Coordinate
	HashMap<Pawn, MapCoordinate> AssignedWorkers;
	// The Zone and other classes needed for accessability checks
	Zone ExcavationZone;
	transient PathManager paths;
	MovementModality Modality;

	public ExcavateJob(GameMap map) {
		Designations = new HashMap<ChunkCoordinate, BlockShape[]>();
		AccessibleExcavations = new HashMap<ChunkCoordinate, BitSet>();
		AssignedExcavations = new HashMap<ChunkCoordinate, BitSet>();

		Name = "Delving Deeply";
		AccessibleMap = new HashMap<MapCoordinate, MapCoordinate>();
		AssignedWorkers = new HashMap<Pawn, MapCoordinate>();

		CurrentBlockShape = new BlockShape(); 
		AdjacentBlockShape = new BlockShape();

		Modality = new MovementModality(MovementModality.MovementType.MOVEMENT_TYPE_WALK, 1, 1);
		Priority = 10;
		paths = PathManager.getSingleton();
	}

	public void addDesignations(VolumeSelection Selection, Zone SourceZone, BlockShape NewShape) {
		this.ExcavationZone = SourceZone;
		MapCoordinate Origin = Selection.OriginLocation;
		MapCoordinate Terminal = Selection.TerminalLocation;
		MapCoordinate TargetCoords = new MapCoordinate();

		for (int x = Origin.getX(); x < Terminal.getX() + 1; x++) {
			for (int y = Origin.getY(); y < Terminal.getY() + 1; y++) {
				for (int z = Origin.getZ(); z < Terminal.getZ() + 1; z++) {
					TargetCoords.set(x, y, z);
					ChunkCoordinate ChunkCoords = TargetCoords.Chunk;
					int BlockIndex = TargetCoords.Block.getBlockIndex();

					BlockShape[] DesignationShapes = Designations.get(ChunkCoords);
					BitSet AccessibleLocation = AccessibleExcavations.get(ChunkCoords);
					BitSet AssignedLocations = AssignedExcavations.get(ChunkCoords);

					GameMap.getMap().getBlockShape(TargetCoords, CurrentBlockShape);
					if (CurrentBlockShape != NewShape) {
						if (DesignationShapes == null) {
							DesignationShapes = new BlockShape[BlockCoordinate.BLOCKS_PER_CHUNK];
							Designations.put(ChunkCoords, DesignationShapes);
						}
						DesignationShapes[BlockIndex] = NewShape;
						DesignationCount++;

						if (AccessibleLocation == null) {
							AccessibleLocation = new BitSet(BlockCoordinate.BLOCKS_PER_CHUNK);
							AccessibleExcavations.put(ChunkCoords, AccessibleLocation);
						}
						if (AssignedLocations == null) {
							AssignedLocations = new BitSet(BlockCoordinate.BLOCKS_PER_CHUNK);
							AssignedExcavations.put(ChunkCoords, AssignedLocations);
						}

						if (!CurrentBlockShape.isSolid()) {
							BitSet DirectionFlags = paths.getDirectionFlags(TargetCoords, Modality);
							if (DirectionFlags.cardinality() != 0) {
								AccessibleMap.put(TargetCoords.clone(), TargetCoords.clone());
								AccessibleExcavationCount++;
								//break;
							}
						}

						// Test for Accesability of this Coordinate
						BitSet DirectionFlags;
						for (Direction dir : Direction.AXIAL_DIRECTIONS) {
							MapCoordinate AdjacentcCoords = TargetCoords.clone();
							AdjacentcCoords.translate(dir);
							GameMap.getMap().getBlockShape(AdjacentcCoords, AdjacentBlockShape);

							if (AdjacentBlockShape.isEmpty() || (dir == Direction.DIRECTION_DOWN && !AdjacentBlockShape.isSolid())) {
								DirectionFlags = paths.getDirectionFlags(AdjacentcCoords, Modality);
								if (DirectionFlags.cardinality() != 0) {
									AccessibleLocation.set(BlockIndex);
									AccessibleMap.put(TargetCoords.clone(), AdjacentcCoords.clone());
									AccessibleExcavationCount++;
									break;
								}
							}
						}

						AccessibleLocation.set(BlockIndex, false);
						AssignedLocations.set(BlockIndex, false);
					}
				}
			}
		}
	}

	public BlockShape getDesignation(MapCoordinate Coords) {
		MapCoordinate ChunkCoords = Coords.clone();
		BlockShape[] DesignationShapes = Designations.get(Coords.Chunk);
		return (DesignationShapes != null) ? DesignationShapes[Coords.Block.getBlockIndex()] : null;
	}

	public void completeDesignation(MapCoordinate Coords) {
		AccessibleMap.remove(Coords);
		ExcavationZone.removeMapCoordinate(Coords);

		DesignationCount--;
		AccessibleExcavationCount--;
		AssignedExcavationsCount--;

		// test adjacent for new accesability
		if (paths.getDirectionFlags(Coords, Modality).cardinality() > 0) {
			BitSet DirectionFlags;
			for (Direction dir : Direction.AXIAL_DIRECTIONS) {
				MapCoordinate AdjacentcCoords = Coords.clone();
				AdjacentcCoords.translate(dir);
				GameMap.getMap().getBlockShape(AdjacentcCoords, AdjacentBlockShape);
				BlockShape DesignationShape = getDesignation(AdjacentcCoords);

				if (DesignationShape != null && !DesignationShape.isExcavationEquivilent(AdjacentBlockShape)) {
					BitSet AccessibleLocation = AccessibleExcavations.get(Coords.Chunk);
					AccessibleLocation.set(Coords.Block.getBlockIndex());
					AccessibleMap.put(AdjacentcCoords.clone(), Coords.clone());
					AccessibleExcavationCount++;
				}
			}
		}

		if (DesignationCount == 0) {
			Manager.terminateJob(this);
		} else {
			if (AccessibleExcavationCount == 0 && AssignedExcavationsCount == 0) {
				this.Paused = true;
			}
		}
	}

	public boolean isAssigned(MapCoordinate Coords) {
		BitSet Assignments = AssignedExcavations.get(Coords.Chunk);
		return Assignments.get(Coords.Block.getBlockIndex());
	}

	public Task nextTask(Pawn IdlePawn) {
		Task OldTask = IdlePawn.getTask();
		if (OldTask.ParentJob == this) {
			if (OldTask.type == Task.TaskType.TASK_GOTO) {
				MapCoordinate TargetExcavation = AssignedWorkers.get(IdlePawn);

				Task newTask = new Task(this, Task.TaskType.TASK_DIG, TargetExcavation);
				return newTask;
			}
		}

		MapCoordinate ExcavateBlock = null;
		MapCoordinate AccsibleBlock = null;

		for (Map.Entry<MapCoordinate, MapCoordinate> entry : AccessibleMap.entrySet()) {
			// distance test for best location
			if (!isAssigned(entry.getKey())) {
				AccsibleBlock = entry.getValue();
				ExcavateBlock = entry.getKey();
				break;
			}
		}

		if (ExcavateBlock != null) {
			AssignedWorkers.put(IdlePawn, ExcavateBlock);
			BitSet Assignments = AssignedExcavations.get(ExcavateBlock.Chunk);
			Assignments.set(ExcavateBlock.Block.getBlockIndex());
			AssignedExcavationsCount++;

			Task newTask = new Task(this, Task.TaskType.TASK_GOTO, AccsibleBlock);
			return newTask;
		}

		if (!needsWorkers()) {
			return Manager.idleCitizen(IdlePawn);
		} else {
			IdlePawn.onBreak = true;
			return IdlePawn.findTask();
		}
	}

	public boolean needsWorkers() {
		return (AccessibleExcavationCount > AssignedExcavationsCount) ? true : false;
	}

	public float evaluatePawn(Pawn CandidateCitizen) {
		float Evaluation = 0;
		if (Workers.contains(CandidateCitizen))
			Evaluation += 1;

		// distance test
		return Evaluation + 2;
	}

	public void finishJob() {
	}

	// this method is used by serialization
	private void readObject(ObjectInputStream ois) throws ClassNotFoundException, IOException {
		// default deserialization
		ois.defaultReadObject();
		// fix transients
		paths = PathManager.getSingleton();
	}
}