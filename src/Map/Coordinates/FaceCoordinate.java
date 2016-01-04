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

import java.io.Serializable;

/**
 * Describes the location of a Face, a face is normally the 2D space between
 * two map cube volumes but slopes can occour inside cubic volumes and are identified
 * with a NONE direction or angular directions, short Coordinates refere to the Cells 
 * array of cubes, a CellCoordinate is thus needed to fully resolve the map location of a Face
 *
 * @author Impaler
 */
public class FaceCoordinate extends BlockCoordinate implements Serializable {

	private static final long serialVersionUID = 1;
	public byte FaceDirection;

	FaceCoordinate() {
		super((byte) 0);
		FaceDirection = (byte) Direction.DIRECTION_DESTINATION.ordinal();
	}

	public FaceCoordinate(BlockCoordinate Index, Direction TargetDirection) {
		super(Index.DetailLevel, Index.Data);
		FaceDirection = (byte) TargetDirection.ordinal();
	}

	public FaceCoordinate(FaceCoordinate FaceCoords) {
		super(FaceCoords);
		FaceDirection = FaceCoords.FaceDirection;
	}

	public void set(FaceCoordinate ArgumentCoordinates) {
		super.copy(ArgumentCoordinates);
		FaceDirection = ArgumentCoordinates.FaceDirection;
	}

	public void set(short CubeIndex, Direction DirectionComponent) {
		super.set(CubeIndex);
		FaceDirection = (byte) DirectionComponent.ordinal();
	}

	public void set(int NewX, int NewY, int Z, Direction DirectionComponent) {
		super.set(NewX, NewY, NewY);
		FaceDirection = (byte) DirectionComponent.ordinal();
	}

	public short getCoordinates() {
		return super.getBlockIndex();
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

		return (Arg.DetailLevel == this.DetailLevel && Arg.Data == this.Data && Arg.FaceDirection == this.FaceDirection);
	}

	@Override
	public int hashCode() {
		int Key = Data;
		Key <<= 10;
		Key += FaceDirection;

		return Key;
	}
}
