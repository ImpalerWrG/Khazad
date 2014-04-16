/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Job;

import java.util.HashMap;
import java.util.BitSet;

import Game.Pawn;
import Map.CellCoordinate;
import Map.MapCoordinate;
import Map.CubeShape;
import Interface.VolumeSelection;

/**
 *
 * @author Impaler
 */
public class ExcavateJob extends Job {

	HashMap<CellCoordinate, CubeShape[]> Designations;
	HashMap<CellCoordinate, BitSet> Accessible;
	HashMap<CellCoordinate, BitSet> Assigned;
	
			
	public ExcavateJob() {
		Designations = new HashMap<CellCoordinate, CubeShape[]>();
		Accessible = new HashMap<CellCoordinate, BitSet>();
		Assigned = new HashMap<CellCoordinate, BitSet>();
	}

	public void addDesignations(VolumeSelection Selection, CubeShape NewShape) {
		MapCoordinate Origin = Selection.OriginLocation;
		MapCoordinate Terminal = Selection.TerminalLocation;

		for (int x = Origin.X; x < Terminal.X + 1; x++) {
			for (int y = Origin.Y; y < Terminal.Y + 1; y++) {
				for (int z = Origin.Z; z < Terminal.Z + 1; z++) {
					MapCoordinate ZoneCube = new MapCoordinate(x, y, z);
					CellCoordinate CellCoords = new CellCoordinate(ZoneCube);
					int CubeIndex = ZoneCube.CubeInt();

					CubeShape[] DesignationShapes = Designations.get(CellCoords);
					BitSet AccessibleLocation = Accessible.get(CellCoords);
					BitSet AssignedLocations = Assigned.get(CellCoords);
					
					if (DesignationShapes == null)
						DesignationShapes = new CubeShape[MapCoordinate.CUBESPERCELL];
						Designations.put(CellCoords, DesignationShapes);

					if (AccessibleLocation == null)
						AccessibleLocation = new BitSet(MapCoordinate.CUBESPERCELL);
						Accessible.put(CellCoords, AccessibleLocation);

					if (AssignedLocations == null)
						AssignedLocations = new BitSet(MapCoordinate.CUBESPERCELL);
						Assigned.put(CellCoords, AssignedLocations);

					DesignationShapes[CubeIndex] = NewShape;
					AccessibleLocation.set(CubeIndex, false);
					AssignedLocations.set(CubeIndex, false);
				}
			}
		}
	}
	
	public Task nextTask(Pawn IdleCitizen) {
		return null;
	}
	
	public float EvaluatePawn(Pawn IdleCitizen) {
		return 1.0f;
	}

}
