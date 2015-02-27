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
 * Describes the location of a Face, a face is normally the 2D space between 
 * two map cube volumes but slopes can occour inside cubic volumes and are identified
 * with a NONE direction, byte Coordinates refere to the Cells array of 256 cubes,
 * a CellCoordinate is thus needed to fully resolve the map location of a Face
 * @author Impaler
 */
public class FaceCoordinate {
	
	public byte Coordinates;
	public Direction FaceDirection;
	
	FaceCoordinate() {
		Coordinates = 0;
		FaceDirection = Direction.DIRECTION_DESTINATION;
	}
	
	public FaceCoordinate(byte CubeCoordinates, Direction TargetDirection) {
		Coordinates = CubeCoordinates;
		FaceDirection = TargetDirection;		
	}

	public void set(FaceCoordinate ArgumentCoordinates) {
		Coordinates = ArgumentCoordinates.Coordinates;
		FaceDirection = ArgumentCoordinates.FaceDirection;
	}
		
	public void Set(byte TargetCoords, Direction DirectionComponent) {
		Coordinates = TargetCoords;
		FaceDirection = DirectionComponent;
	}

	public void Set(int NewX, int NewY, Direction DirectionComponent) {
		Coordinates = (byte) ((NewX << MapCoordinate.CELLBITSHIFT) + NewY);
		FaceDirection = DirectionComponent;
	}
	
	public int getX() {
		return (Coordinates >> MapCoordinate.CELLBITSHIFT) & 15;
	}

	public int getY() {
		return (Coordinates & MapCoordinate.CELLBITFLAG) & 15;
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
		int Key =  Coordinates;
		Key <<= 8;
		Key += FaceDirection.ordinal();

		return Key;
	}
}

