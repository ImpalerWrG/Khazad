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

import Map.CubeCoordinate;

/**
 *
 * @author Impaler
 */
public class VolumeSelection {

	// End Points of the Bounding Box
	public CubeCoordinate OriginLocation = new CubeCoordinate();
	public CubeCoordinate TerminalLocation = new CubeCoordinate();
	// Used by Rendering
	public boolean Dirty;

	public VolumeSelection() {
		Dirty = true;
	}

	public VolumeSelection(CubeCoordinate SelectionOrigin, CubeCoordinate SelectionTerminus) {
		setSize(SelectionOrigin, SelectionTerminus);
		Dirty = true;
	}

	public final void setSize(CubeCoordinate SelectionOrigin, CubeCoordinate SelectionTerminus) {

		int maxX = Math.max(SelectionTerminus.X, SelectionOrigin.X);
		int maxY = Math.max(SelectionTerminus.Y, SelectionOrigin.Y);
		int maxZ = Math.max(SelectionTerminus.Z, SelectionOrigin.Z);

		int minX = Math.min(SelectionTerminus.X, SelectionOrigin.X);
		int minY = Math.min(SelectionTerminus.Y, SelectionOrigin.Y);
		int minZ = Math.min(SelectionTerminus.Z, SelectionOrigin.Z);

		OriginLocation.set(minX, minY, minZ);
		TerminalLocation.set(maxX, maxY, maxZ);

		Dirty = true;
	}
}
