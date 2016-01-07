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

package Interface;

import Map.Coordinates.MapCoordinate;

/**
 *
 * @author Impaler
 */
public class VolumeSelection {

	// End Points of the Bounding Box
	public MapCoordinate OriginLocation = new MapCoordinate();
	public MapCoordinate TerminalLocation = new MapCoordinate();
	// Used by Rendering
	public boolean Dirty;

	public VolumeSelection() {
		Dirty = true;
	}

	public VolumeSelection(MapCoordinate SelectionOrigin, MapCoordinate SelectionTerminus) {
		setSize(SelectionOrigin, SelectionTerminus);
		Dirty = true;
	}

	public final void setSize(MapCoordinate SelectionOrigin, MapCoordinate SelectionTerminus) {

		int maxX = Math.max(SelectionTerminus.getX(), SelectionOrigin.getX());
		int maxY = Math.max(SelectionTerminus.getY(), SelectionOrigin.getY());
		int maxZ = Math.max(SelectionTerminus.getZ(), SelectionOrigin.getZ());

		int minX = Math.min(SelectionTerminus.getX(), SelectionOrigin.getX());
		int minY = Math.min(SelectionTerminus.getY(), SelectionOrigin.getY());
		int minZ = Math.min(SelectionTerminus.getZ(), SelectionOrigin.getZ());

		OriginLocation.set(minX, minY, minZ);
		TerminalLocation.set(maxX, maxY, maxZ);

		Dirty = true;
	}
}
