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

package Map.Coordinates;

import com.jme3.math.Vector3f;
import java.io.Serializable;

/**
 * Used to refrence Cells in HashMaps by relative position, X and Y values are 16
 * times the true Map Coordinate, Z values are equal.
 *
 * @author Impaler
 */
public class ChunkCoordinate implements Serializable {

	private static final long serialVersionUID = 1;
	public short X, Y, Z;

	public ChunkCoordinate() {
		X = Y = Z = 0;
	}

	public ChunkCoordinate(int NewX, int NewY, int NewZ) {
		X = (short) NewX;
		Y = (short) NewY;
		Z = (short) NewZ;
	}

	public void copy(ChunkCoordinate ArgumentCoordinates) {
		X = ArgumentCoordinates.X;
		Y = ArgumentCoordinates.Y;
		Z = ArgumentCoordinates.Z;
	}

	@Override
	public ChunkCoordinate clone() {
		ChunkCoordinate newCoords = new ChunkCoordinate();
		newCoords.copy(this);
		return newCoords;
	}

	@Override
	public boolean equals(Object ArgumentCoordinates) {
		/*
		if (ArgumentCoordinates == null)
			return false;
		if (ArgumentCoordinates == this)
			return true;
		if (!(ArgumentCoordinates instanceof CellCoordinate))
			return false;*/

		ChunkCoordinate Arg = (ChunkCoordinate) ArgumentCoordinates;
		return (X == Arg.X && Y == Arg.Y && Z == Arg.Z);
	}

	@Override
	public int hashCode() {
		int Key = 0;

		Key += X;
		Key <<= 12;
		Key += Y;
		Key <<= 12;
		Key += Z;

		return Key;
	}

	public Vector3f getVector() {
		float x = (float) (X * BlockCoordinate.CHUNK_EDGE_SIZE);
		float y = (float) (Y * BlockCoordinate.CHUNK_EDGE_SIZE);
		float z = (float) (Z * BlockCoordinate.CHUNK_EDGE_SIZE);

		if(x < 0)
			x++;
		if(y < 0)
			y++;
		if(z < 0)
			z++;

		return new Vector3f(x, y, z);
	}

	@Override
	public String toString() {
		return "X " + Integer.toString(X) + "Y " + Integer.toString(Y) + "Z " + Integer.toString(Z);
	}
}