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

package PathFinding;

import Map.Cell;
import java.util.BitSet;
import java.util.ArrayList;
import java.util.Collections;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedDeque;

import Map.Axis;
import Map.GameMap;
import Map.MapCoordinate;
import Map.CellCoordinate;
import Map.Direction;
import Map.CubeShape;
import java.io.Serializable;

/**
 * The primary implementation of Grid for Khazad pathfinding, it uses a GridCell
 * class the spacially corresponds to the MapCell class for interchangability of
 * Coordinates, but the GridCell holds a single large BitSet of ~7000 bits that
 * corresponds to every Coordinates possible edge to neibhors. Also it records
 * a connectivity zone for each coordinate.
 *
 * The Grid class then stores a HashMap of these GridCells, again mirroring the
 * structure used in GameMap, as well as a double HashMap and List which are
 * built in a single read pass on the Game Map, this allows for a constant time
 * connection query but is not dynamic.
 *
 * @author Impaler
 */
public class KhazadGrid implements GridInterface, Serializable {

	private static final long serialVersionUID = 1;

	protected class GridCell {

		private BitSet DirectionMatrix;
		int[] ConnectivityZone;
		private CellCoordinate thisCellCoodinates;

		GridCell(CellCoordinate Coordinates) {
			thisCellCoodinates = Coordinates;
			DirectionMatrix = new BitSet(MapCoordinate.CUBESPERCELL * Direction.ANGULAR_DIRECTIONS.length);
			ConnectivityZone = new int[MapCoordinate.CUBESPERCELL];
		}

		public int getConnectivityZone(int Cube) {
			return ConnectivityZone[Cube];
		}

		public void setConnectivityZone(int Cube, int Zone) {
			ConnectivityZone[Cube] = Zone;
		}

		public BitSet getCubeDirections(int Cube) {
			return DirectionMatrix.get(Cube * Direction.ANGULAR_DIRECTIONS.length, ((Cube + 1) * Direction.ANGULAR_DIRECTIONS.length));
		}

		public void setCubeDirection(int Cube, Direction TargetDirection, boolean newValue) {
			DirectionMatrix.set((Cube * Direction.ANGULAR_DIRECTIONS.length) + TargetDirection.ordinal(), newValue);
		}

		void setCubeDirections(int Cube, BitSet ArgumentSet) {
			for (int i = 0; i < Direction.ANGULAR_DIRECTIONS.length; i++) {
				DirectionMatrix.set((Cube * Direction.ANGULAR_DIRECTIONS.length) + i, ArgumentSet.get(i));
			}
		}

		CellCoordinate getCellCoordinates() {
			return thisCellCoodinates;
		}
	}

	ConcurrentHashMap<CellCoordinate, GridCell> GridCells;
	ConcurrentLinkedDeque<MapCoordinate> DirtyLocations;
	// Connections between groups of Coordinates
	ArrayList<Integer> ConnectivityCache;
	ConcurrentHashMap<Integer, ConcurrentHashMap<Integer, Integer>> ConnectivityMap;
	// The unique mix of movements this Grid is modeling
	MovementModality GridModality;
	GameMap SourceMap;

	public KhazadGrid(GameMap TargetMap, MovementModality Modality) {
		GridCells = new ConcurrentHashMap<CellCoordinate, GridCell>();
		ConnectivityMap = new ConcurrentHashMap<Integer, ConcurrentHashMap<Integer, Integer>>();
		ConnectivityCache = new ArrayList<Integer>();
		DirtyLocations = new ConcurrentLinkedDeque<MapCoordinate>();
		GridModality = Modality;
		SourceMap = TargetMap;

		for (Cell TargetCell : TargetMap.getCellCollection()) {
			if (TargetCell != null) {
				CellCoordinate CellCoords = TargetCell.getCellCoordinates();
				GridCell NewGridCell = addCell(CellCoords);

				byte TargetCube = 0;
				do {
					MapCoordinate TargetCoords = new MapCoordinate(CellCoords, TargetCube);
					BitSet Flags = buildConnectivitySet(TargetCoords);
					NewGridCell.setCubeDirections(TargetCube & 0xFF, Flags);
					TargetCube++;
				} while (TargetCube != 0);  // End Loop when Byte rolls over
			}
		}

		buildConnectivityZones();
	}

	void buildConnectivityZones() {
		int ZoneCounter = 0;

		// Loop to do connectivity
		for (GridCell TargetCell : GridCells.values()) {
			if (TargetCell != null) {
				CellCoordinate CellCoords = TargetCell.getCellCoordinates();

				byte TargetCube = 0;
				do {
					BitSet Flags = getDirectionEdgeSet(new MapCoordinate(CellCoords, TargetCube));

					if (Flags.cardinality() > 0) {
						if (TargetCell.ConnectivityZone[TargetCube & 0xFF] == 0) { // Start a new zone if not connected to another zone
							ZoneCounter++; // First zone will be 1, 0 will indicate un-ititialized
							TargetCell.ConnectivityZone[TargetCube & 0xFF] = ZoneCounter;
						}
						// Push this current zone onto the adjacent area
						int CurrentZoneIndex = TargetCell.ConnectivityZone[TargetCube & 0xFF];

						for (Direction dir : Direction.ANGULAR_DIRECTIONS) {
							if (Flags.get(dir.ordinal())) {
								// Find the Zone that the adjcent Tile has
								MapCoordinate AdjacentTileCoords = new MapCoordinate(CellCoords, TargetCube);
								AdjacentTileCoords.translate(dir);
								int AdjacentZoneIndex = getConnectivityZone(AdjacentTileCoords);

								if (AdjacentZoneIndex == 0) {// The other location is unititialized
									setConnectivityZone(AdjacentTileCoords, CurrentZoneIndex); // Push this current zone onto the adjacent area
								} else {
									if (AdjacentZoneIndex != CurrentZoneIndex) { // A different zone update the Connectivity Map
										changeConnectivityMap(CurrentZoneIndex, AdjacentZoneIndex, 1);  // Add one connection between zones
									}
								}
							}
						}
					} else {
						TargetCell.ConnectivityZone[TargetCube & 0xFF] = 0;
					}
					TargetCube++;
				} while (TargetCube != 0);  // End Loop when Byte rolls over
			}
		}

		RebuildConnectivityCache(ZoneCounter);
	}

	private BitSet buildConnectivitySet(MapCoordinate TargetCoords) {
		BitSet Flags = new BitSet(MapCoordinate.CUBESPERCELL);
		CubeShape TargetShape = SourceMap.getCubeShape(TargetCoords);

		if (!TargetShape.isSky() && !TargetShape.hasCeiling()) {
			MapCoordinate OverheadTileCoords = TargetCoords.clone();
			OverheadTileCoords.translate(Direction.DIRECTION_UP);
			CubeShape OverheadCube = SourceMap.getCubeShape(OverheadTileCoords);
			boolean OverheadPassable = !OverheadCube.isSolid();

			for (Direction dir : Direction.ANGULAR_DIRECTIONS) {
				MapCoordinate AdjacentTileCoords = TargetCoords.clone();
				AdjacentTileCoords.translate(dir);
				CubeShape AdjacentCubeShape = SourceMap.getCubeShape(AdjacentTileCoords);

				if (!AdjacentCubeShape.isSky() && !AdjacentCubeShape.hasCeiling()) {
					if (dir.getValueonAxis(Axis.AXIS_Z) == 1) {
						if (OverheadPassable) {
							Flags.set(dir.ordinal());
						}
					} else {
						//If no vertical direction, we only care that this tile is passable
						Flags.set(dir.ordinal());
					}
				}
			}
		}
		return Flags;
	}

	public BitSet getDirectionEdgeSet(MapCoordinate TargetCoords) {
		GridCell TargetCell = getCell(new CellCoordinate(TargetCoords));
		if (TargetCell != null) {
			return TargetCell.getCubeDirections(TargetCoords.getCubeIntIndex());
		}
		return new BitSet();  // No connectivity because Cell is invalid
	}

	public boolean isEdge(MapCoordinate TargetCoords, Direction DirectionType) {
		GridCell TargetCell = getCell(new CellCoordinate(TargetCoords));
		if (TargetCell != null) {
			int Positition = ((TargetCoords.getCubeByteIndex() & 0xFF) * Direction.ANGULAR_DIRECTIONS.length) + DirectionType.ordinal();
			return TargetCell.DirectionMatrix.get(Positition);
		}
		return false;
	}

	public int getConnectivityZone(MapCoordinate TargetCoords) {
		GridCell TargetCell = getCell(new CellCoordinate(TargetCoords));
		if (TargetCell != null) {
			return TargetCell.ConnectivityZone[TargetCoords.getCubeByteIndex() & 0xFF];
		}
		return 0;  // No connectivity zone because Cell is invalid
	}

	void setConnectivityZone(MapCoordinate TargetCoords, int NewZone) {
		GridCell TargetCell = getCell(new CellCoordinate(TargetCoords));
		if (TargetCell != null) {
			TargetCell.ConnectivityZone[TargetCoords.getCubeByteIndex() & 0xFF] = NewZone;
		}
	}

	void setDirectionFlags(MapCoordinate MapCoords, BitSet Flags) {
		//find the Target cell (add if necessary)
		GridCell TargetCell = addCell(new CellCoordinate(MapCoords));

		TargetCell.setCubeDirections(MapCoords.getCubeByteIndex() & 0xFF, Flags);
	}

	ConcurrentHashMap<Integer, Integer> getConnectivtySubMap(int Zone) {
		ConcurrentHashMap<Integer, Integer> SubMap = ConnectivityMap.get(Zone);

		if (SubMap == null) {
			SubMap = new ConcurrentHashMap<Integer, Integer>();
			ConnectivityMap.put(Zone, SubMap);
			return SubMap;
		}
		return SubMap;
	}

	void changeConnectivityMap(int FirstZone, int SecondZone, int connectionChange) {
		if (FirstZone == SecondZone) {
			return;  // Cannot connect to self
		}

		ConcurrentHashMap<Integer, Integer> TargetMap = getConnectivtySubMap(FirstZone);
		Integer it = TargetMap.get(SecondZone);

		if (it == null) {
			if (connectionChange > 0) { // Do not allow negative connection counts
				TargetMap.put(SecondZone, connectionChange);
			}
		} else {
			if ((it.intValue() + connectionChange) <= 0) {
				TargetMap.remove(it); // Remove the connection entirely
			}
		}
	}

	public boolean contains(MapCoordinate TestCoords) {
		return getCell(new CellCoordinate(TestCoords)) != null;
	}

	GridCell getCell(CellCoordinate TestCoords) {
		return GridCells.get(TestCoords);
	}

	GridCell addCell(CellCoordinate TargetCoords) {
		GridCell TargetCell = getCell(TargetCoords);
		if (TargetCell == null) {
			GridCell NewGridCell = new GridCell(TargetCoords);
			GridCells.put(TargetCoords, NewGridCell);
			return NewGridCell;
		}
		return TargetCell;
	}

	public float getEdgeCost(MapCoordinate TestCoords, Direction DirectionType) {
		if (isEdge(TestCoords, DirectionType)) {
			if (DirectionType.getValueonAxis(Axis.AXIS_Z) != 0) // True for Up and Down
				return 2;

			boolean X = DirectionType.getValueonAxis(Axis.AXIS_X) != 0;
			boolean Y = DirectionType.getValueonAxis(Axis.AXIS_Y) != 0;
			final float root2 = (float) Math.sqrt(2);

			if (X ^ Y) // N, S, E, W
				return 1;
			if (X & Y) {
				return root2;
			}

			if (DirectionType == Direction.DIRECTION_NONE)
				return 0;
			return 2;  // All other z axis diagonals
		}
		return -1;  // No Edge exists
	}

	public boolean isPathPossible(MovementModality MovementType, MapCoordinate StartCoords, MapCoordinate GoalCoords) {
		return getZoneEquivilency(StartCoords) == getZoneEquivilency(GoalCoords);
	}

	public void dirtyMapCoordinate(MapCoordinate[] DirtyCoords) {
		Collections.addAll(DirtyLocations, DirtyCoords);

		do {
			MapCoordinate TargetCoords = DirtyLocations.poll();
			BitSet NewConnectivitySet = buildConnectivitySet(TargetCoords);

			CellCoordinate TargetCell = new CellCoordinate(TargetCoords);
			GridCell TargetGridCell = getCell(TargetCell);
			BitSet CurrentConnectivity = getDirectionEdgeSet(TargetCoords);
			int SourceZone = getConnectivityZone(TargetCoords);

			for (Direction dir : Direction.ANGULAR_DIRECTIONS) {
				boolean NewConnectionValue = NewConnectivitySet.get(dir.ordinal());
				MapCoordinate AdjacentTileCoords = TargetCoords.clone();
				AdjacentTileCoords.translate(dir);
				CellCoordinate AdjacentCell = new CellCoordinate(AdjacentTileCoords);
				GridCell AdjacentGridCell = getCell(AdjacentCell);
				int AdjacentZone = getConnectivityZone(AdjacentTileCoords);

				if (NewConnectionValue != CurrentConnectivity.get(dir.ordinal())) {

					if (NewConnectionValue) { // Connection created
						if (SourceZone == 0 && AdjacentZone != 0) {  // Match the zone any adjacent connected zone
							TargetGridCell.setConnectivityZone(TargetCoords.getCubeIntIndex(), AdjacentZone);
						}

						if (AdjacentZone == 0 && SourceZone != 0) {  // Alternativly push the existing zone into unitialized space
							TargetGridCell.setConnectivityZone(AdjacentTileCoords.getCubeIntIndex(), SourceZone);
						}
					}

					if (SourceZone != AdjacentZone) {
						if (NewConnectionValue) {
							changeConnectivityMap(SourceZone, AdjacentZone, 1);
						} else {
							changeConnectivityMap(SourceZone, AdjacentZone, -1);
						}
					}
					AdjacentGridCell.setCubeDirection(AdjacentTileCoords.getCubeIntIndex(), dir.invert(), NewConnectionValue);
					SourceMap.getCell(AdjacentCell).setDirtyPathingRendering(true);
				}
			}
			TargetGridCell.setCubeDirections(TargetCoords.getCubeIntIndex(), NewConnectivitySet);
			SourceMap.getCell(TargetCell).setDirtyPathingRendering(true);

		} while (!DirtyLocations.isEmpty());
	}

	int getZoneEquivilency(MapCoordinate TargetCoords) {
		return ConnectivityCache.get(getConnectivityZone(TargetCoords));
	}

	int getZoneEquivilency(int Zone) {
		return ConnectivityCache.get(Zone);
	}

	void updateConnectivityCache(int Zone, int ZoneEquivilency) {
		ConnectivityCache.set(Zone, ZoneEquivilency);

		ConcurrentHashMap<Integer, Integer> TargetMap = getConnectivtySubMap(Zone);
		for (Integer i : TargetMap.keySet()) {
			if (ConnectivityCache.get(i.intValue()) == 0) {
				updateConnectivityCache(i, ZoneEquivilency);
			}
		}
	}

	void RebuildConnectivityCache(int ZoneCount) {
		ConnectivityCache = new ArrayList<Integer>(Collections.nCopies(ZoneCount + 1, 0));

		for (int i : ConnectivityCache) {  // Skip zero, no such zone		
			if (ConnectivityCache.get(i) == 0) {
				updateConnectivityCache(i, i);
			}
		}
	}

	public ArrayList<MapCoordinate> getPassableCoordinates() {
		ArrayList<MapCoordinate> TestCoords = new ArrayList<MapCoordinate>();
		for (KhazadGrid.GridCell TargetCell : GridCells.values()) {
			CellCoordinate CellCoords = TargetCell.getCellCoordinates();

			BitSet DirectionSet;
			for (int TargetCube = 0; TargetCube < 256; TargetCube++) {
				DirectionSet = TargetCell.getCubeDirections(TargetCube);
				if (DirectionSet.cardinality() != 0) {  // Any Valid Edge Exists
					MapCoordinate TestCoordinates = new MapCoordinate(CellCoords, (byte) TargetCube);
					TestCoords.add(TestCoordinates);
				}
			}
		}
		return TestCoords;
	}
}
