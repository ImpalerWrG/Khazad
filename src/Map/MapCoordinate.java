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
 * Fundemental coordinate for describing the cubic voxel space of the game play
 * map.  Can be constructed from numerous other coordinate combinations and
 * translated to adjacent cubes by Direction enums, a very common operation.
 * @author Impaler
 */
public final class MapCoordinate implements Cloneable{

	public static final int CELLEDGESIZE = 16;
	public static final int CELLBITSHIFT = 4;
	public static final int CELLBITFLAG = 15;
	public static final int CUBESPERCELL = 256;
	public static final float HALFCUBE = (float)0.5;
	  
	public int X, Y, Z;

	public MapCoordinate() {
		X = Y = Z = 0;
	}

	public MapCoordinate(CellCoordinate CellCoords, byte CubeCoords) {
		X = (CellCoords.X * CELLEDGESIZE) + ((CubeCoords >> CELLBITSHIFT) & CELLBITFLAG);
		Y = (CellCoords.Y * CELLEDGESIZE) + (CubeCoords & CELLBITFLAG);
		Z = CellCoords.Z;
	}

	public MapCoordinate(CellCoordinate CellCoords, int X, int Y) {
		this.X = (CellCoords.X * CELLEDGESIZE) + X;
		this.Y = (CellCoords.Y * CELLEDGESIZE) + Y;
		Z = CellCoords.Z;
	}

	public MapCoordinate(MapCoordinate SourceCoords, Direction DirectionType) {
		X = SourceCoords.X + DirectionType.ValueonAxis(Axis.AXIS_X);
		Y = SourceCoords.Y + DirectionType.ValueonAxis(Axis.AXIS_Y);
		Z = SourceCoords.Z + DirectionType.ValueonAxis(Axis.AXIS_Z);
	}

	public void TranslateMapCoordinates(Direction DirectionType) {
		X += DirectionType.ValueonAxis(Axis.AXIS_X);
		Y += DirectionType.ValueonAxis(Axis.AXIS_Y);
		Z += DirectionType.ValueonAxis(Axis.AXIS_Z);
	}

	public void TranslateMapCoordinates(Direction DirectionType, int Length) {
		X += DirectionType.ValueonAxis(Axis.AXIS_X) * Length;
		Y += DirectionType.ValueonAxis(Axis.AXIS_Y) * Length;
		Z += DirectionType.ValueonAxis(Axis.AXIS_Z) * Length;
	}

	public MapCoordinate(int NewX, int NewY, int NewZ) {
		Set(NewX, NewY, NewZ);
	}

	public void Set(int NewX, int NewY, int NewZ) {
		X = NewX;   Y = NewY;   Z = NewZ;
	}

	public void Set(Axis AxialComponent, int NewValue) {
		switch (AxialComponent)
		{
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

	public byte CubeByteIndex() {
		byte tempx = (byte) ((X & CELLBITFLAG) << CELLBITSHIFT);
		byte tempy = ((byte) (Y & CELLBITFLAG));
		return (byte) (tempx + tempy);
	}

	public int CubeIntIndex() {
		return  ((X & CELLBITFLAG) << CELLBITSHIFT) + (Y & CELLBITFLAG);
	}

	public int CubeInt() {
		int tempx = (X & CELLBITFLAG) << CELLBITSHIFT;
		int tempy = Y & CELLBITFLAG;
		return tempx + tempy;
	}

	public void copy(MapCoordinate ArgumentCoordinates) {
		X = ArgumentCoordinates.X;
		Y = ArgumentCoordinates.Y;
		Z = ArgumentCoordinates.Z;
	}

	@Override
	public MapCoordinate clone() {
		MapCoordinate newCoords = new MapCoordinate();
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
        if (!(ArgumentCoordinates instanceof MapCoordinate))
            return false;*/
		
		MapCoordinate Arg = (MapCoordinate) ArgumentCoordinates;
		return (Arg.X == this.X && Arg.Y == this.Y && Arg.Z == this.Z);
	}

	public int ValueonAxis(Axis AxialComponent) {
		switch (AxialComponent)
		{
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

