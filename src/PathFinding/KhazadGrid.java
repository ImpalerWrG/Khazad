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
import java.util.HashMap;
import java.util.Collections;
import java.util.concurrent.ConcurrentHashMap;

import Map.Axis;
import Map.GameMap;
import Map.MapCoordinate;
import Map.CellCoordinate;
import Map.Direction;
import Map.CubeShape;

/**
 * The primary implementation of Grid for Khazad pathfinding, it uses a GridCell
 * class the spacially corresponds to the MapCell class for interchangability of
 * Coordinates, but the GridCell holds a single large BitSet of ~7000 bits that
 * corresponds to every Coordinates possible edge to neibhors.  Also it records
 * a connectivity zone for each coordinate.
 * 
 * The Grid class then stores a HashMap of these GridCells, again mirroring the
 * structure used in GameMap, as well as a double HashMap and List which are 
 * built in a single read pass on the Game Map, this allows for a constant time
 * connection query but is not dynamic.
 * 
 * @author Impaler
 */
public class KhazadGrid implements GridInterface {
	
	protected class GridCell {
		private BitSet DirectionMatrix;
		int[] ConnectivityZone;

		private CellCoordinate thisCellCoodinates;

		GridCell(CellCoordinate Coordinates) {
			thisCellCoodinates = Coordinates;
			DirectionMatrix = new BitSet(MapCoordinate.CUBESPERCELL * Direction.ANGULAR_DIRECTIONS.length);
			ConnectivityZone = new int[MapCoordinate.CUBESPERCELL];	
		}
		
		public BitSet getCubeDirections(int Cube) {
			return DirectionMatrix.get(Cube * Direction.ANGULAR_DIRECTIONS.length, ((Cube + 1) * Direction.ANGULAR_DIRECTIONS.length));
		}

		void setCubeDirections(int Cube, BitSet ArgumentSet) {
			for (int i = ArgumentSet.nextSetBit(0); i >= 0; i = ArgumentSet.nextSetBit(i + 1)) {
				DirectionMatrix.set((Cube * Direction.ANGULAR_DIRECTIONS.length) + i, true);
			}
		}
		
		CellCoordinate getCellCoordinates() {
			return thisCellCoodinates;
		}
	}
	
	public ConcurrentHashMap<CellCoordinate, GridCell> GridCells;
	ArrayList<MapCoordinate> DirtyLocations;
    ArrayList<Integer> ConnectivityCache;
	HashMap<Integer, HashMap<Integer, Integer> > ConnectivityMap;
	MovementModality GridModality;
	GameMap SourceMap;
	
	public KhazadGrid(GameMap TargetMap, MovementModality Modality) {
		GridCells = new ConcurrentHashMap<CellCoordinate, GridCell>();
		ConnectivityMap = new HashMap<Integer, HashMap<Integer, Integer> >();
		ConcurrentHashMap<CellCoordinate, Cell> cells = TargetMap.getCellMap();
		ConnectivityCache = new ArrayList<Integer>();
		DirtyLocations = new ArrayList<MapCoordinate>();
		GridModality = Modality;
		SourceMap = TargetMap;
		
		for (Cell TargetCell : cells.values()) {
			if (TargetCell != null) {
				CellCoordinate CellCoords = TargetCell.getCellCoordinates();
				GridCell NewGridCell = addCell(CellCoords);

				byte TargetCube = 0;
				do {
					MapCoordinate TargetCoords = new MapCoordinate(CellCoords, TargetCube);
					BitSet Flags = BuildConnectivitySet(TargetCoords);
					NewGridCell.setCubeDirections(TargetCube & 0xFF, Flags);
					TargetCube++;
				} while (TargetCube != 0);  // End Loop when Byte rolls over
			}
		}

		BuildConnectivityZones();
	}

	void BuildConnectivityZones() {
		int ZoneCounter = 0;

		// Loop to do connectivity
		for (GridCell TargetCell: GridCells.values()) {
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

						for (Direction dir: Direction.ANGULAR_DIRECTIONS) {
							if (Flags.get(dir.ordinal())) {
								// Find the Zone that the adjcent Tile has
								MapCoordinate AdjacentTileCoords = new MapCoordinate(CellCoords, TargetCube);
								AdjacentTileCoords.TranslateMapCoordinates(dir);
								int AdjacentZoneIndex = getConnectivityZone(AdjacentTileCoords);

								if (AdjacentZoneIndex == 0) {// The other location is unititialized
									setConnectivityZone(AdjacentTileCoords, CurrentZoneIndex); // Push this current zone onto the adjacent area
								} else {
									if (AdjacentZoneIndex != CurrentZoneIndex) { // A different zone update the Connectivity Map
										ChangeConnectivityMap(CurrentZoneIndex, AdjacentZoneIndex, 1);  // Add one connection between zones
									}
								}
							}
						}
					} else {
						TargetCell.ConnectivityZone[TargetCube & 0xFF] = 0;
					}
					TargetCube++;
				}
				while (TargetCube != 0);  // End Loop when Byte rolls over
			}
		}

		RebuildConnectivityCache(ZoneCounter);
	}

	private BitSet BuildConnectivitySet(MapCoordinate TargetCoords) {
		BitSet Flags = new BitSet(MapCoordinate.CUBESPERCELL);
		CubeShape TargetShape = SourceMap.getCubeShape(TargetCoords);

		if (!TargetShape.isSky() && !TargetShape.isSolid()) {
			MapCoordinate OverheadTileCoords = TargetCoords.clone();
			OverheadTileCoords.TranslateMapCoordinates(Direction.DIRECTION_UP);
			CubeShape OverheadCube = SourceMap.getCubeShape(OverheadTileCoords);
			boolean OverheadPassable = !OverheadCube.isSolid();

			for (Direction dir: Direction.ANGULAR_DIRECTIONS) {
				MapCoordinate AdjacentTileCoords = TargetCoords.clone();
				AdjacentTileCoords.TranslateMapCoordinates(dir);
				Direction InvertedDirection = dir.Invert();

				// if we've done this already..
				if (getDirectionEdgeSet(AdjacentTileCoords).get(InvertedDirection.ordinal())) {
					Flags.set(dir.ordinal());
					continue;
				}

				CubeShape AdjacentCubeShape = SourceMap.getCubeShape(AdjacentTileCoords);

				if (!AdjacentCubeShape.isSky() && !AdjacentCubeShape.isSolid()) {
					if (dir.ValueonAxis(Axis.AXIS_Z) == 1) {
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
			return TargetCell.getCubeDirections(TargetCoords.CubeIndex() & 0xFF);
		}
		return new BitSet();  // No connectivity because Cell is invalid
	}

	public boolean isEdge(MapCoordinate TargetCoords, Direction DirectionType) {
		GridCell TargetCell = getCell(new CellCoordinate(TargetCoords));
		if (TargetCell != null) {
			int Positition = ((TargetCoords.CubeIndex() & 0xFF) * Direction.ANGULAR_DIRECTIONS.length)  + DirectionType.ordinal();
			return TargetCell.DirectionMatrix.get(Positition);
		}
		return false;
	}

	int getConnectivityZone(MapCoordinate TargetCoords) {
		GridCell TargetCell = getCell(new CellCoordinate(TargetCoords));
		if (TargetCell != null) {
			return TargetCell.ConnectivityZone[TargetCoords.CubeIndex() & 0xFF];
		}
		return 0;  // No connectivity zone because Cell is invalid
	}

	void setConnectivityZone(MapCoordinate TargetCoords, int NewZone) {
		GridCell TargetCell = getCell(new CellCoordinate(TargetCoords));
		if (TargetCell != null) {
			TargetCell.ConnectivityZone[TargetCoords.CubeIndex() & 0xFF] = NewZone;
		}
	}

	void setDirectionFlags(MapCoordinate MapCoords, BitSet Flags) {
		//find the Target cell (add if necessary)
		GridCell TargetCell = addCell(new CellCoordinate(MapCoords));

		TargetCell.setCubeDirections(MapCoords.CubeIndex() & 0xFF, Flags);
	}

	HashMap<Integer, Integer> getConnectivtySubMap(int Zone) {
		HashMap<Integer, Integer> SubMap = ConnectivityMap.get(Zone);

		if (SubMap == null) {
			SubMap = new HashMap<Integer, Integer>();
			ConnectivityMap.put(Zone, SubMap);
			return SubMap;
		}
		return SubMap;
	}

	void ChangeConnectivityMap(int FirstZone, int SecondZone, int connectionChange) {
		if (FirstZone == SecondZone) {
			return;  // Cannot connect to self
		}

		HashMap<Integer, Integer> TargetMap = getConnectivtySubMap(FirstZone);
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
		if (TargetCell == null)
		{
			GridCell NewGridCell = new GridCell(TargetCoords);
			GridCells.put(TargetCoords, NewGridCell);
			return NewGridCell;
		}
		return TargetCell;
	}

	public float getEdgeCost(MapCoordinate TestCoords, Direction DirectionType) {
		if (isEdge(TestCoords, DirectionType)) {
			if (DirectionType.ValueonAxis(Axis.AXIS_Z) != 0) // True for Up and Down
				return 2;

			boolean X = DirectionType.ValueonAxis(Axis.AXIS_X) != 0;
			boolean Y = DirectionType.ValueonAxis(Axis.AXIS_Y) != 0;
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

	 public void DirtyMapCoordinate(MapCoordinate[] TargetCoords) {
		 Collections.addAll(DirtyLocations, TargetCoords);
	 }

	int getZoneEquivilency(MapCoordinate TargetCoords) {
		return ConnectivityCache.get(getConnectivityZone(TargetCoords));
	}

	void updateConnectivityCache(int Zone, int ZoneEquivilency) {
		ConnectivityCache.set(Zone, ZoneEquivilency);

		HashMap<Integer, Integer> TargetMap = getConnectivtySubMap(Zone);
		for (Integer i: TargetMap.keySet()) {
			if (ConnectivityCache.get(i.intValue()) == 0) {
				updateConnectivityCache(i, ZoneEquivilency);
			}
		}
	}

	void RebuildConnectivityCache(int ZoneCount) {
		ConnectivityCache = new ArrayList<Integer>(Collections.nCopies(ZoneCount + 1, 0));

		for (int i: ConnectivityCache) {  // Skip zero, no such zone		
			if (ConnectivityCache.get(i) == 0) {
				updateConnectivityCache(i, i);
			}
		}
	}
	
	public ArrayList<MapCoordinate> getPassableCoordinates() {
		ArrayList<MapCoordinate> TestCoords = new ArrayList<MapCoordinate>();
		for (KhazadGrid.GridCell TargetCell : GridCells.values()) {
			CellCoordinate CellCoords = TargetCell.getCellCoordinates();
			//int TargetCube = 0;
			BitSet DirectionSet;
			for (int TargetCube = 0; TargetCube < 256; TargetCube++) {
				DirectionSet = TargetCell.getCubeDirections(TargetCube);
				if (DirectionSet.cardinality() != 0) {  // Any Valid Edge Exists
					MapCoordinate TestCoordinates = new MapCoordinate(CellCoords, (byte) TargetCube);
					TestCoords.add(TestCoordinates);
				}
			}
		}  // End Loop when Byte rolls over
		return TestCoords;
	}
}
