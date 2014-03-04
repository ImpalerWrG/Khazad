/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Map;

import java.util.*;
import Interface.VolumeSelection;
/**
 *
 * @author Impaler
 */
public class Zone {

	HashMap<CellCoordinate, BitSet> ZoneMap;

	public Zone(List<VolumeSelection> Volumes) {
		for (VolumeSelection Selection : Volumes) {
			addSelection(Selection);
		}
	}

	public final void addSelection(VolumeSelection Selection) {
		MapCoordinate Origin = Selection.OriginLocation;
		MapCoordinate Terminal = Selection.TerminalLocation;

		for (int x = Origin.X; x < Terminal.X; x++)
		{
			for (int y = Origin.Y; y < Terminal.Y; y++)
			{
				for (int z = Origin.Z; z < Terminal.Z; z++)
				{
					MapCoordinate ZoneCube = new MapCoordinate(x, y, z);
					CellCoordinate ZoneCell = new CellCoordinate(ZoneCube);

					BitSet Target = ZoneMap.get(ZoneCell);
					if (Target != null)
					{
						Target.set(ZoneCube.Cube(), true);
					} else {
						BitSet Bits = new BitSet(MapCoordinate.CUBESPERCELL);
						Bits.set(ZoneCube.Cube(), true);
						ZoneMap.put(ZoneCell, Bits);
					}
				}
			}
		}
	}

	boolean isCoordinateInZone(MapCoordinate TestCoordinates) {
		CellCoordinate Targt = new CellCoordinate(TestCoordinates);
		BitSet Target = ZoneMap.get(Targt);
		if (Target != null) {
			return (Target.get(TestCoordinates.Cube()));
		} else {
			return false;
		}
	}
}
