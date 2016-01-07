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

import Map.Coordinates.BlockCoordinate;
import Map.Coordinates.MapCoordinate;
import Map.Coordinates.ChunkCoordinate;
import java.util.*;
import Interface.VolumeSelection;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.Serializable;

/**
 * Basic description of a volume in the game Map, high efficiency storage by bitset
 * mapped to ChunkCoordinates to give fast query, zones have no trouble overlapping
 *
 * @author Impaler
 */
public class Zone implements Serializable {

	private static final long serialVersionUID = 1;
	HashMap<ChunkCoordinate, BitSet> ZoneMap;
	transient public boolean Dirty;
	private final int ID;

	public Zone(List<VolumeSelection> Volumes, int ID) {
		ZoneMap = new HashMap<ChunkCoordinate, BitSet>();
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
		MapCoordinate TargetCoords = new MapCoordinate();

		for (int x = Origin.getX(); x < Terminal.getX(); x++) {
			for (int y = Origin.getY(); y < Terminal.getY(); y++) {
				for (int z = Origin.getZ(); z < Terminal.getZ(); z++) {
					TargetCoords.set(x, y, z);

					BitSet Target = ZoneMap.get(TargetCoords.Chunk);
					if (Target != null) {
						Target.set(TargetCoords.Block.getBlockIndex(), true);
					} else {
						BitSet Bits = new BitSet(BlockCoordinate.BLOCKS_PER_CHUNK);
						Bits.set(TargetCoords.Block.getBlockIndex(), true);
						ZoneMap.put(TargetCoords.Chunk, Bits);
					}
				}
			}
		}
		Dirty = true;
	}

	public final void addMapCoordinate(MapCoordinate AdditionCoords) {
		BitSet Target = ZoneMap.get(AdditionCoords.Chunk);
		if (Target == null) {
			Target = new BitSet(BlockCoordinate.BLOCKS_PER_CHUNK);
		}
		Target.set(AdditionCoords.Block.getBlockIndex(), true);
		Dirty = true;
	}

	public final void removeSelection(VolumeSelection Selection) {
		MapCoordinate Origin = Selection.OriginLocation;
		MapCoordinate Terminal = Selection.TerminalLocation;
		MapCoordinate TargetCoords = new MapCoordinate();

		for (int x = Origin.getX(); x < Terminal.getX(); x++) {
			for (int y = Origin.getY(); y < Terminal.getY(); y++) {
				for (int z = Origin.getZ(); z < Terminal.getZ(); z++) {
					TargetCoords.set(x, y, z);

					BitSet Target = ZoneMap.get(TargetCoords.Chunk);
					if (Target != null) {
						Target.set(TargetCoords.Block.getBlockIndex(), false);
					}
				}
			}
		}

		for (Map.Entry<ChunkCoordinate, BitSet> entry : ZoneMap.entrySet()) {
			BitSet ChunkBitSet = entry.getValue();
			if (ChunkBitSet.cardinality() == 0)
				ZoneMap.remove(entry.getKey());
		}

		Dirty = true;
	}

	public final void removeMapCoordinate(MapCoordinate RemovalCoords) {
		BitSet Target = ZoneMap.get(RemovalCoords.Chunk);
		if (Target != null) {
			Target.clear(RemovalCoords.Block.getBlockIndex());
			Dirty = true;
		}
	}

	boolean isCoordinateInZone(MapCoordinate TestCoordinates) {
		BitSet Target = ZoneMap.get(TestCoordinates.Chunk);
		if (Target != null) {
			return (Target.get(TestCoordinates.Block.getBlockIndex()));
		} else {
			return false;
		}
	}

	public HashMap<ChunkCoordinate, BitSet> getZoneMap() {
		return ZoneMap;
	}

	public int getID() {
		return ID;
	}
}
