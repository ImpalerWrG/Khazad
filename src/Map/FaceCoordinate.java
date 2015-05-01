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
 * Describes the location of a Face, a face is normally the 2D space between
 * two map cube volumes but slopes can occour inside cubic volumes and are identified
 * with a NONE direction, byte Coordinates refere to the Cells array of 256 cubes,
 * a CellCoordinate is thus needed to fully resolve the map location of a Face
 *
 * @author Impaler
 */
public class FaceCoordinate implements Serializable {

	private static final long serialVersionUID = 1;
	public byte Coordinates;
	public byte FaceDirection;

	FaceCoordinate() {
		Coordinates = 0;
		FaceDirection = (byte) Direction.DIRECTION_DESTINATION.ordinal();
	}

	public FaceCoordinate(byte CubeCoordinates, Direction TargetDirection) {
		Coordinates = CubeCoordinates;
		FaceDirection = (byte) TargetDirection.ordinal();
	}

	public void set(FaceCoordinate ArgumentCoordinates) {
		Coordinates = ArgumentCoordinates.Coordinates;
		FaceDirection = ArgumentCoordinates.FaceDirection;
	}

	public void set(byte TargetCoords, Direction DirectionComponent) {
		Coordinates = TargetCoords;
		FaceDirection = (byte) DirectionComponent.ordinal();
	}

	public void set(int NewX, int NewY, Direction DirectionComponent) {
		Coordinates = (byte) ((NewX << CubeCoordinate.CELLBITSHIFT) + NewY);
		FaceDirection = (byte) DirectionComponent.ordinal();
	}

	public int getX() {
		return (Coordinates >> CubeCoordinate.CELLBITSHIFT) & 15;
	}

	public int getY() {
		return (Coordinates & CubeCoordinate.CELLBITFLAG) & 15;
	}

	public Direction getFaceDirection() {
		return Direction.ANGULAR_DIRECTIONS[FaceDirection];
	}

	@Override
	public boolean equals(Object ArgumentCoordinates) {
		if (ArgumentCoordinates == null)
			return false;
		if (ArgumentCoordinates == this)
			return true;
		if (!(ArgumentCoordinates instanceof FaceCoordinate))
			return false;

		FaceCoordinate Arg = (FaceCoordinate) ArgumentCoordinates;

		return (Arg.Coordinates == Coordinates && Arg.FaceDirection == FaceDirection);
	}

	@Override
	public int hashCode() {
		int Key = Coordinates;
		Key <<= 8;
		Key += FaceDirection;

		return Key;
	}
}
