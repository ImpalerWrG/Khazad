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

import java.io.Serializable;

/**
 * Fundemental coordinate for describing the cubic voxel space of the game play
 * map. Can be constructed from numerous other coordinate combinations and
 * translated to adjacent cubes by Direction enums, a very common operation.
 *
 * @author Impaler
 */
public final class CubeCoordinate implements Cloneable, Serializable {

	private static final long serialVersionUID = 1;
	public static final int CELLEDGESIZE = 16;
	public static final int CELLBITSHIFT = 4;
	public static final int CELLBITFLAG = 15;
	public static final int CUBESPERCELL = 256;
	public static final int CELLDETAILLEVELS = 5;
	public static final float HALFCUBE = (float) 0.5;
	// Primary values
	public int X, Y, Z;

	public CubeCoordinate() {
		X = Y = Z = 0;
	}

	public CubeCoordinate(CellCoordinate CellCoords, byte CubeCoords) {
		X = (CellCoords.X * CELLEDGESIZE) + ((CubeCoords >> CELLBITSHIFT) & CELLBITFLAG);
		Y = (CellCoords.Y * CELLEDGESIZE) + (CubeCoords & CELLBITFLAG);
		Z = CellCoords.Z;
	}

	public CubeCoordinate(CellCoordinate CellCoords, int X, int Y) {
		this.X = (CellCoords.X * CELLEDGESIZE) + X;
		this.Y = (CellCoords.Y * CELLEDGESIZE) + Y;
		Z = CellCoords.Z;
	}

	public CubeCoordinate(CubeCoordinate SourceCoords, Direction DirectionType) {
		X = SourceCoords.X + DirectionType.getValueonAxis(Axis.AXIS_X);
		Y = SourceCoords.Y + DirectionType.getValueonAxis(Axis.AXIS_Y);
		Z = SourceCoords.Z + DirectionType.getValueonAxis(Axis.AXIS_Z);
	}

	public CubeCoordinate(int NewX, int NewY, int NewZ) {
		set(NewX, NewY, NewZ);
	}

	public void translate(Direction DirectionType) {
		X += DirectionType.getValueonAxis(Axis.AXIS_X);
		Y += DirectionType.getValueonAxis(Axis.AXIS_Y);
		Z += DirectionType.getValueonAxis(Axis.AXIS_Z);
	}

	public void translate(Direction DirectionType, int Length) {
		X += DirectionType.getValueonAxis(Axis.AXIS_X) * Length;
		Y += DirectionType.getValueonAxis(Axis.AXIS_Y) * Length;
		Z += DirectionType.getValueonAxis(Axis.AXIS_Z) * Length;
	}

	public void set(int NewX, int NewY, int NewZ) {
		X = NewX;
		Y = NewY;
		Z = NewZ;
	}

	public void set(Axis AxialComponent, int NewValue) {
		switch (AxialComponent) {
			case AXIS_Z:
				Z = NewValue;
				break;
			case AXIS_Y:
				Y = NewValue;
				break;
			case AXIS_X:
				X = NewValue;
				break;

			default:
				break;
		}
	}

	public byte getCubeByteIndex() {
		byte tempx = (byte) ((X & CELLBITFLAG) << CELLBITSHIFT);
		byte tempy = ((byte) (Y & CELLBITFLAG));
		return (byte) (tempx + tempy);
	}

	public int getCubeIntIndex() {
		return ((X & CELLBITFLAG) << CELLBITSHIFT) + (Y & CELLBITFLAG);
	}

	public void copy(CubeCoordinate ArgumentCoordinates) {
		X = ArgumentCoordinates.X;
		Y = ArgumentCoordinates.Y;
		Z = ArgumentCoordinates.Z;
	}

	@Override
	public CubeCoordinate clone() {
		CubeCoordinate newCoords = new CubeCoordinate();
		newCoords.copy(this);
		return newCoords;
	}

	@Override
	public boolean equals(Object ArgumentCoordinates) {

		//if (ArgumentCoordinates == null)
		//return false;
		//if (ArgumentCoordinates == this)
		//return true;
		//if (!(ArgumentCoordinates instanceof MapCoordinate))
		//return false;

		CubeCoordinate Arg = (CubeCoordinate) ArgumentCoordinates;
		return (Arg.X == this.X && Arg.Y == this.Y && Arg.Z == this.Z);
	}

	public int getValueonAxis(Axis AxialComponent) {
		switch (AxialComponent) {
			case AXIS_Z:
				return Z;
			case AXIS_Y:
				return Y;
			case AXIS_X:
				return X;
			default:
				return 0;
		}
	}

	@Override
	public int hashCode() {
		int hash = 3;
		hash += 17 * X;
		hash += 37 * Y;
		hash += 5 * Z;
		return hash;
	}
}
