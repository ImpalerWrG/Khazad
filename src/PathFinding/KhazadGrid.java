/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
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
		
		BitSet getCubeDirections(int Cube) {
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

    //MapCoordinate maxlen;
    //MapCoordinate minlen;

    ArrayList<Integer> ConnectivityCache;
    HashMap<Integer, HashMap<Integer, Integer> > ConnectivityMap;

	
	public KhazadGrid(GameMap TargetMap) {
		GridCells = new ConcurrentHashMap<CellCoordinate, GridCell>();
		ConnectivityMap = new HashMap<Integer, HashMap<Integer, Integer> >();
		HashMap<CellCoordinate, Cell> cells = TargetMap.getCellMap();
		ConnectivityCache = new ArrayList<Integer>();
		
		for (Cell TargetCell : cells.values()) {
			if (TargetCell != null)
			{
				CellCoordinate CellCoords = TargetCell.getCellCoordinates();
				GridCell NewGridCell = addCell(CellCoords);

				byte TargetCube = 0;
				do
				{
					CubeShape Shape = TargetCell.getCubeShape((byte) TargetCube);
					BitSet Flags = new BitSet(MapCoordinate.CUBESPERCELL);

					if (!Shape.isSky() && !Shape.isSolid()) {
						MapCoordinate OverheadTileCoords = new MapCoordinate(CellCoords, TargetCube);
						OverheadTileCoords.TranslateMapCoordinates(Direction.DIRECTION_UP);
						CubeShape OverheadCube = TargetMap.getCubeShape(OverheadTileCoords);
						boolean OverheadPassable = OverheadCube.isSky();

						for (Direction dir: Direction.ANGULAR_DIRECTIONS) {
							MapCoordinate AdjacentTileCoords = new MapCoordinate(CellCoords, TargetCube);
							AdjacentTileCoords.TranslateMapCoordinates(dir);
							Direction InvertedDirection = dir.Invert();

							// if we've done this already..
							if (getDirectionFlags(AdjacentTileCoords).get(InvertedDirection.ordinal())) {
								Flags.set(dir.ordinal());
								continue;
							}

							CubeShape AdjacentCubeShape = TargetMap.getCubeShape(AdjacentTileCoords);

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

						NewGridCell.setCubeDirections(TargetCube & 0xFF, Flags);
					}
					TargetCube++;
				}
				while (TargetCube != 0);  // End Loop when Byte rolls over
			}
		}

		BuildConnectivityZones();
	}

	void BuildConnectivityZones() {
		int ZoneCounter = 0;

		// Loop to do connectivity
		for (GridCell TargetCell: GridCells.values()) {
			if (TargetCell != null) {
				CellCoordinate CellCoords = TargetCell.thisCellCoodinates;

				byte TargetCube = 0;
				do {
					BitSet Flags = getDirectionFlags(new MapCoordinate(CellCoords, TargetCube));

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

	public BitSet getDirectionFlags(MapCoordinate TargetCoords) {
		GridCell TargetCell = getCell(new CellCoordinate(TargetCoords));
		if (TargetCell != null) {
			return TargetCell.getCubeDirections(TargetCoords.Cube() & 0xFF);
		}
		return new BitSet();  // No connectivity because Cell is invalid
	}

	public boolean isEdge(MapCoordinate TargetCoords, Direction DirectionType) {
		GridCell TargetCell = getCell(new CellCoordinate(TargetCoords));
		if (TargetCell != null) {
			int Positition = ((TargetCoords.Cube() & 0xFF) * Direction.ANGULAR_DIRECTIONS.length)  + DirectionType.ordinal();
			return TargetCell.DirectionMatrix.get(Positition);
		}
		return false;
	}

	int getConnectivityZone(MapCoordinate TargetCoords) {
		GridCell TargetCell = getCell(new CellCoordinate(TargetCoords));
		if (TargetCell != null) {
			return TargetCell.ConnectivityZone[TargetCoords.Cube() & 0xFF];
		}
		return 0;  // No connectivity zone because Cell is invalid
	}

	void setConnectivityZone(MapCoordinate TargetCoords, int NewZone) {
		GridCell TargetCell = getCell(new CellCoordinate(TargetCoords));
		if (TargetCell != null) {
			TargetCell.ConnectivityZone[TargetCoords.Cube() & 0xFF] = NewZone;
		}
	}

	void setDirectionFlags(MapCoordinate MapCoords, BitSet Flags) {
		//find the Target cell (add if necessary)
		GridCell TargetCell = addCell(new CellCoordinate(MapCoords));

		TargetCell.setCubeDirections(MapCoords.Cube() & 0xFF, Flags);

		//for (Axis AxisType: Axis.values()) {
		//	if (MapCoords[AxisType.ordinal()] > maxlen[AxisType.ordinal()]) {
		//		maxlen.Set(AxisType.ordinal(), MapCoords[AxisType.ordinal()]);
		//	}
		//	if (MapCoords[AxisType.ordinal()] < minlen[AxisType.ordinal()]) {
		//		minlen.Set(AxisType.ordinal(), MapCoords[AxisType.ordinal()]);
		//	}
		//}
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
			if (DirectionType.Z != 0) // True for Up and Down
				return 2;

			boolean X = DirectionType.X != 0;
			boolean Y = DirectionType.Y != 0;
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

	public boolean isPathPossible(MapCoordinate StartCoords, MapCoordinate GoalCoords) {
		return getZoneEquivilency(StartCoords) == getZoneEquivilency(GoalCoords);
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
}
