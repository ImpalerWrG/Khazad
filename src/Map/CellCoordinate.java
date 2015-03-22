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

/**
 * Used to refrence Cells in HashMaps by relative position, X and Y values are 16
 * times the true Map Coordinate, Z values are equal.
 * @author Impaler
 */
public class CellCoordinate {

	public short X, Y, Z;
	
	public CellCoordinate() {
		X = Y = Z = 0;
	}

	public CellCoordinate(int NewX, int NewY, int NewZ) {
		X = (short) NewX;
		Y = (short) NewY;
		Z = (short) NewZ;
	}

	public CellCoordinate(MapCoordinate SourceCoordinates) {
		if (SourceCoordinates.X >= 0) {
			X = (short) (SourceCoordinates.X >> MapCoordinate.CELLBITSHIFT);
		} else {
			X = (short) ((SourceCoordinates.X >> MapCoordinate.CELLBITSHIFT) - 1); //truncate to negative infinity
		}
		if (SourceCoordinates.Y >= 0) {
			Y = (short) (SourceCoordinates.Y >> MapCoordinate.CELLBITSHIFT);
		} else {
			Y = (short) ((SourceCoordinates.Y >> MapCoordinate.CELLBITSHIFT) - 1); //truncate to negative infinity
		}
		Z = (short) SourceCoordinates.Z;
	}

	public void setequal(CellCoordinate ArgumentCoordinates) {
		X = ArgumentCoordinates.X;
		Y = ArgumentCoordinates.Y;
		Z = ArgumentCoordinates.Z;
	}

	public boolean notequal(CellCoordinate ArgumentCoordinates) {
		return (X != ArgumentCoordinates.X || Y != ArgumentCoordinates.Y || Z != ArgumentCoordinates.Z);
	}

	public void copy(CellCoordinate ArgumentCoordinates) {
		X = ArgumentCoordinates.X;
		Y = ArgumentCoordinates.Y;
		Z = ArgumentCoordinates.Z;
	}

	@Override
	public CellCoordinate clone() {
		CellCoordinate newCoords = new CellCoordinate();
		newCoords.copy(this);
		return newCoords;
	}

	@Override
	public boolean equals(Object ArgumentCoordinates) {
		if (ArgumentCoordinates == null)
			return false;
		if (ArgumentCoordinates == this)
			return true;
		if (!(ArgumentCoordinates instanceof CellCoordinate))
			return false;
		
		CellCoordinate Arg = (CellCoordinate) ArgumentCoordinates;
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
	
	@Override
	public String toString() {
		return  "X " + Integer.toString(X) + "Y " + Integer.toString(Y) + "Z " + Integer.toString(Z);
	}
}