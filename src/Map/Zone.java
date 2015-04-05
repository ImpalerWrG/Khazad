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

package Map;

import java.util.*;
import Interface.VolumeSelection;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.Serializable;

/**
 * Basic description of a volume in the game Map, high efficiency storage by bitset
 * mapped to CellCoordinates to give fast query, zones have no trouble overlapping
 *
 * @author Impaler
 */
public class Zone implements Serializable {

	private static final long serialVersionUID = 1;
	HashMap<CellCoordinate, BitSet> ZoneMap;
	transient public boolean Dirty;
	private final int ID;

	public Zone(List<VolumeSelection> Volumes, int ID) {
		ZoneMap = new HashMap<CellCoordinate, BitSet>();
		for (VolumeSelection Selection : Volumes) {
			addSelection(Selection);
		}
		this.ID = ID;
		Dirty = true;
	}

	private void readObject(ObjectInputStream ois) throws ClassNotFoundException, IOException {
		// default deserialization
		ois.defaultReadObject();
		// fix transients
		Dirty = true;
	}

	public final void addSelection(VolumeSelection Selection) {
		MapCoordinate Origin = Selection.OriginLocation;
		MapCoordinate Terminal = Selection.TerminalLocation;

		for (int x = Origin.X; x <= Terminal.X; x++) {
			for (int y = Origin.Y; y <= Terminal.Y; y++) {
				for (int z = Origin.Z; z <= Terminal.Z; z++) {

					MapCoordinate ZoneCube = new MapCoordinate(x, y, z);
					CellCoordinate ZoneCell = new CellCoordinate(ZoneCube);

					BitSet Target = ZoneMap.get(ZoneCell);
					if (Target != null) {
						Target.set(ZoneCube.getCubeIntIndex(), true);
					} else {
						BitSet Bits = new BitSet(MapCoordinate.CUBESPERCELL);
						Bits.set(ZoneCube.getCubeIntIndex(), true);
						ZoneMap.put(ZoneCell, Bits);
					}
				}
			}
		}
		Dirty = true;
	}

	public final void addMapCoordinate(MapCoordinate AdditionCoords) {
		CellCoordinate Targt = new CellCoordinate(AdditionCoords);
		BitSet Target = ZoneMap.get(Targt);
		if (Target == null) {
			Target = new BitSet(MapCoordinate.CUBESPERCELL);
		}
		Target.set(AdditionCoords.getCubeIntIndex(), true);
		Dirty = true;
	}

	public final void removeSelection(VolumeSelection Selection) {
		MapCoordinate Origin = Selection.OriginLocation;
		MapCoordinate Terminal = Selection.TerminalLocation;

		for (int x = Origin.X; x < Terminal.X; x++) {
			for (int y = Origin.Y; y < Terminal.Y; y++) {
				for (int z = Origin.Z; z < Terminal.Z; z++) {

					MapCoordinate ZoneCube = new MapCoordinate(x, y, z);
					CellCoordinate ZoneCell = new CellCoordinate(ZoneCube);

					BitSet Target = ZoneMap.get(ZoneCell);
					if (Target != null) {
						Target.set(ZoneCube.getCubeByteIndex(), false);
					}
				}
			}
		}

		for (Map.Entry<CellCoordinate, BitSet> entry : ZoneMap.entrySet()) {
			BitSet CellBitSet = entry.getValue();
			if (CellBitSet.cardinality() == 0)
				ZoneMap.remove(entry.getKey());
		}

		Dirty = true;
	}

	public final void removeMapCoordinate(MapCoordinate RemovalCoords) {
		CellCoordinate Targt = new CellCoordinate(RemovalCoords);
		BitSet Target = ZoneMap.get(Targt);
		if (Target != null) {
			Target.clear(RemovalCoords.getCubeIntIndex());
			Dirty = true;
		}
	}

	boolean isCoordinateInZone(MapCoordinate TestCoordinates) {
		CellCoordinate Targt = new CellCoordinate(TestCoordinates);
		BitSet Target = ZoneMap.get(Targt);
		if (Target != null) {
			return (Target.get(TestCoordinates.getCubeByteIndex()));
		} else {
			return false;
		}
	}

	public HashMap<CellCoordinate, BitSet> getZoneMap() {
		return ZoneMap;
	}

	public int getID() {
		return ID;
	}
}
