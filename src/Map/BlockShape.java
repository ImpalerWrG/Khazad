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

import Map.Coordinates.*;
import java.io.Serializable;

/**
 * Holds corner height values used to determine sloped shapes and Meshes
 * of map tiles
 *
 * @author Impaler
 */
public class BlockShape implements Serializable {

	private static final long serialVersionUID = 1;
	// Defined values for determining how many vertical fractions exist in a cube
	public static final byte BELOW_CUBE_HEIGHT = 0;
	public static final byte CUBE_BOTTOM_HEIGHT = 1;
	public static final byte HEIGHT_FRACTIONS = 4;
	public static final byte CUBE_TOP_HEIGHT = 5;
	// BitPacking  SWW SEE NWW NEE FLAG
	//             FED CBA 987 654 3210
	// Mask values used to extract values for a corner
	public static final short SWMASK = (short) 57344;
	public static final short SEMASK = 7168;
	public static final short NWMASK = 896;
	public static final short NEMASK = 112;
	// Names for the specific Flags and the Mask to retrive them
	public static final short FLAGMASK = 15;
	public static final short SPLITMASK = 1;
	public static final short VERTICALMASK = 2;
	public static final short UNUSED1MASK = 4;
	public static final short UNUSED2MASK = 8;
	// Shifting values used to extract values for a corner
	public static final short SWSHIFT = 13;
	public static final short SESHIFT = 10;
	public static final short NWSHIFT = 7;
	public static final short NESHIFT = 4;
	// Bit compressed heights of each corner and flags
	
	public static final short EMPTY_CUBE_DATA = 0;
	short Data;

	public BlockShape(byte SWCornerHeight, byte SECornerHeight, byte NWCornerHeight, byte NECornerHeight, byte NewFlags) {
		Data = (short) (((SWCornerHeight << SWSHIFT) & SWMASK) | ((SECornerHeight << SESHIFT) & SEMASK) | ((NWCornerHeight << NWSHIFT) & NWMASK) | ((NECornerHeight << NESHIFT) & NEMASK) | (NewFlags & FLAGMASK));
	}

	public BlockShape(byte NewHeight, byte flags) {
		this(NewHeight, NewHeight, NewHeight, NewHeight, flags);
	}

	public BlockShape(byte NewHeight) {
		this(NewHeight, NewHeight, NewHeight, NewHeight, (byte) 0);
	}

	public BlockShape() {
		this(CUBE_TOP_HEIGHT);
	}

	public BlockShape(short newData) {
		Data = newData;
	}

	public short getData() {
		return Data;
	}
	
	public void setData(short newData) {
		this.Data = newData;
	}

	public boolean isSolid() {
		return (getSouthWestCorner() >= CUBE_TOP_HEIGHT) && (getSouthEastCorner() >= CUBE_TOP_HEIGHT) && (getNorthWestCorner() >= CUBE_TOP_HEIGHT) && (getNorthEastCorner() >= CUBE_TOP_HEIGHT);
	}

	public boolean isEmpty() {
		return (getSouthWestCorner() <= CUBE_BOTTOM_HEIGHT) && (getSouthEastCorner() <= CUBE_BOTTOM_HEIGHT) && (getNorthWestCorner() <= CUBE_BOTTOM_HEIGHT) && (getNorthEastCorner() <= CUBE_BOTTOM_HEIGHT);
	}

	public int volume() {
		int sum1, sum2;
		if (isSplit()) {
			sum1 = getNorthWestCorner() + getNorthEastCorner() + getSouthEastCorner() - 3;
			sum2 = getNorthWestCorner() + getSouthWestCorner() + getSouthEastCorner() - 3;
		} else {
			sum1 = getSouthWestCorner() + getNorthWestCorner() + getNorthEastCorner() - 3;
			sum2 = getSouthWestCorner() + getSouthEastCorner() + getNorthEastCorner() - 3;
		}
		return ((Math.min(sum1, 0) + Math.min(sum2, 0))  * 4);
	}

	public boolean isSky() {
		int Counter = 0;

		if (getSouthWestCorner() < CUBE_BOTTOM_HEIGHT)
			Counter++;
		if (getSouthEastCorner() < CUBE_BOTTOM_HEIGHT)
			Counter++;
		if (getNorthEastCorner() < CUBE_BOTTOM_HEIGHT)
			Counter++;
		if (getNorthWestCorner() < CUBE_BOTTOM_HEIGHT)
			Counter++;

		if (Counter >= 2) {
			return true;
		}

		return false;
	}

	public boolean hasFace(Direction DirectionType) {
		if (DirectionType == Direction.DIRECTION_DOWN)
			return hasFloor();
		if (DirectionType == Direction.DIRECTION_UP)
			return hasCeiling();
		if (DirectionType == Direction.DIRECTION_EAST)
			return (getSouthEastCorner() > CUBE_BOTTOM_HEIGHT || getNorthEastCorner() > CUBE_BOTTOM_HEIGHT);
		if (DirectionType == Direction.DIRECTION_WEST)
			return (getSouthWestCorner() > CUBE_BOTTOM_HEIGHT || getNorthWestCorner() > CUBE_BOTTOM_HEIGHT);
		if (DirectionType == Direction.DIRECTION_NORTH)
			return (getNorthEastCorner() > CUBE_BOTTOM_HEIGHT || getNorthWestCorner() > CUBE_BOTTOM_HEIGHT);
		if (DirectionType == Direction.DIRECTION_SOUTH)
			return (getSouthEastCorner() > CUBE_BOTTOM_HEIGHT || getSouthWestCorner() > CUBE_BOTTOM_HEIGHT);

		return false;
	}

	public boolean hasFloor() {
		if (isSplit()) {
			if (getSouthEastCorner() == CUBE_BOTTOM_HEIGHT && getNorthEastCorner() == CUBE_BOTTOM_HEIGHT && getNorthWestCorner() == CUBE_BOTTOM_HEIGHT)
				return true;
			if (getNorthWestCorner() == CUBE_BOTTOM_HEIGHT && getSouthWestCorner() == CUBE_BOTTOM_HEIGHT && getSouthEastCorner() == CUBE_BOTTOM_HEIGHT)
				return true;
			return false;
		} else {
			if (getNorthEastCorner() == CUBE_BOTTOM_HEIGHT && getNorthWestCorner() == CUBE_BOTTOM_HEIGHT && getSouthWestCorner() == CUBE_BOTTOM_HEIGHT)
				return true;
			if (getSouthWestCorner() == CUBE_BOTTOM_HEIGHT && getSouthEastCorner() == CUBE_BOTTOM_HEIGHT && getNorthEastCorner() == CUBE_BOTTOM_HEIGHT)
				return true;
			return false;
		}
	}

	public boolean hasCeiling() {
		if (isSplit()) {
			if (getSouthEastCorner() == CUBE_TOP_HEIGHT && getNorthEastCorner() == CUBE_TOP_HEIGHT && getNorthWestCorner() == CUBE_TOP_HEIGHT)
				return true;
			if (getNorthWestCorner() == CUBE_TOP_HEIGHT && getSouthWestCorner() == CUBE_TOP_HEIGHT && getSouthEastCorner() == CUBE_TOP_HEIGHT)
				return true;
			return false;
		} else {
			if (getNorthEastCorner() == CUBE_TOP_HEIGHT && getNorthWestCorner() == CUBE_TOP_HEIGHT && getSouthWestCorner() == CUBE_TOP_HEIGHT)
				return true;
			if (getSouthWestCorner() == CUBE_TOP_HEIGHT && getSouthEastCorner() == CUBE_TOP_HEIGHT && getNorthEastCorner() == CUBE_TOP_HEIGHT)
				return true;
			return false;
		}
	}

	public byte getSouthWestCorner() {
		return (byte) (((Data & SWMASK) >> SWSHIFT) & 7);
	}

	public byte getSouthEastCorner() {
		return (byte) (((Data & SEMASK) >> SESHIFT) & 7);
	}

	public byte getNorthWestCorner() {
		return (byte) (((Data & NWMASK) >> NWSHIFT) & 7);
	}

	public byte getNorthEastCorner() {
		return (byte) (((Data & NEMASK) >> NESHIFT) & 7);
	}

	public void setSouthWestCorner(byte Height) {
		Data &= ~SWMASK;
		Data |= ((Height << SWSHIFT) & SWMASK);
	}

	public void setSouthEastCorner(byte Height) {
		Data &= ~SEMASK;
		Data |= ((Height << SESHIFT) & SEMASK);
	}

	public void setNorthWestCorner(byte Height) {
		Data &= ~NWMASK;
		Data |= ((Height << NWSHIFT) & NWMASK);
	}

	public void setNorthEastCorner(byte Height) {
		Data &= ~NEMASK;
		Data |= ((Height << NESHIFT) & NEMASK);
	}

	public boolean isSplit() {
		return (Data & SPLITMASK) > 0 ? true : false;
	}

	@Override
	public boolean equals(Object ArgumentShape) {
		BlockShape Arg = (BlockShape) ArgumentShape;
		return Data == Arg.Data;
	}

	public boolean isExcavationEquivilent(Object ArgumentShape) {
		BlockShape Arg = (BlockShape) ArgumentShape;

		boolean NE = Arg.getNorthEastCorner() <= this.getNorthEastCorner();
		boolean SE = Arg.getSouthEastCorner() <= this.getSouthEastCorner();
		boolean NW = Arg.getNorthWestCorner() <= this.getNorthWestCorner();
		boolean SW = Arg.getSouthWestCorner() <= this.getSouthWestCorner();

		return NE && SE && NW && SW;
	}

	public void copy(BlockShape ArgumentShape) {
		Data = ArgumentShape.Data;
	}

	@Override
	public BlockShape clone() {
		BlockShape newShape = new BlockShape();
		newShape.copy(this);
		return newShape;
	}

	public boolean notequal(BlockShape ArgumentShape) {
		return Data != ArgumentShape.Data;
	}

	public float getCenterHeight() {
		if (isSplit()) {
			return (((getNorthWestCorner() + getSouthEastCorner() - 2) / 2.0f) / HEIGHT_FRACTIONS) - BlockCoordinate.HALF_BLOCK;
		} else {
			return (((getSouthWestCorner() + getNorthEastCorner() - 2) / 2.0f) / HEIGHT_FRACTIONS) - BlockCoordinate.HALF_BLOCK;
		}
	}

	public float getDirectionEdgeHeight(Direction DirectionType) {

		switch (DirectionType) {

			case DIRECTION_NORTH:
			case DIRECTION_UP_NORTH:
			case DIRECTION_DOWN_NORTH:
				return (((getNorthWestCorner() + getNorthEastCorner() - 2) / 2.0f) / HEIGHT_FRACTIONS) - BlockCoordinate.HALF_BLOCK;

			case DIRECTION_SOUTH:
			case DIRECTION_UP_SOUTH:
			case DIRECTION_DOWN_SOUTH:
				return (((getSouthWestCorner() + getSouthEastCorner() - 2) / 2.0f) / HEIGHT_FRACTIONS) - BlockCoordinate.HALF_BLOCK;

			case DIRECTION_EAST:
			case DIRECTION_UP_EAST:
			case DIRECTION_DOWN_EAST:
				return (((getNorthEastCorner() + getSouthEastCorner() - 2) / 2.0f) / HEIGHT_FRACTIONS) - BlockCoordinate.HALF_BLOCK;

			case DIRECTION_WEST:
			case DIRECTION_UP_WEST:
			case DIRECTION_DOWN_WEST:
				return (((getNorthWestCorner() + getSouthWestCorner() - 2) / 2.0f) / HEIGHT_FRACTIONS) - BlockCoordinate.HALF_BLOCK;

			case DIRECTION_NORTHWEST:
			case DIRECTION_UP_NORTHWEST:
			case DIRECTION_DOWN_NORTHWEST:
				return ((getNorthWestCorner() - 1.0f) / (float) HEIGHT_FRACTIONS) - BlockCoordinate.HALF_BLOCK;

			case DIRECTION_SOUTHWEST:
			case DIRECTION_UP_SOUTHWEST:
			case DIRECTION_DOWN_SOUTHWEST:
				return ((getSouthWestCorner() - 1.0f) / (float) HEIGHT_FRACTIONS) - BlockCoordinate.HALF_BLOCK;

			case DIRECTION_SOUTHEAST:
			case DIRECTION_UP_SOUTHEAST:
			case DIRECTION_DOWN_SOUTHEAST:
				return ((getSouthEastCorner() - 1.0f) / (float) HEIGHT_FRACTIONS) - BlockCoordinate.HALF_BLOCK;

			case DIRECTION_NORTHEAST:
			case DIRECTION_UP_NORTHEAST:
			case DIRECTION_DOWN_NORTHEAST:
				return ((getNorthEastCorner() - 1.0f) / (float) HEIGHT_FRACTIONS) - BlockCoordinate.HALF_BLOCK;
		}

		return 0;
	}

	public boolean isLightPassable(Axis LightingAxis) {
		switch (LightingAxis) {
			case AXIS_X:
				return false;

			case AXIS_Y:

				return false;
			case AXIS_Z:
				if (isSplit()) {
					if (getSouthEastCorner() <= CUBE_BOTTOM_HEIGHT && getNorthEastCorner() <= CUBE_BOTTOM_HEIGHT && getNorthWestCorner() <= CUBE_BOTTOM_HEIGHT)
						return true;
					if (getNorthWestCorner() <= CUBE_BOTTOM_HEIGHT && getSouthWestCorner() == CUBE_BOTTOM_HEIGHT && getSouthEastCorner() <= CUBE_BOTTOM_HEIGHT)
						return true;
					return false;
				} else {
					if (getNorthEastCorner() <= CUBE_BOTTOM_HEIGHT && getNorthWestCorner() <= CUBE_BOTTOM_HEIGHT && getSouthWestCorner() <= CUBE_BOTTOM_HEIGHT)
						return true;
					if (getSouthWestCorner() <= CUBE_BOTTOM_HEIGHT && getSouthEastCorner() <= CUBE_BOTTOM_HEIGHT && getNorthEastCorner() <= CUBE_BOTTOM_HEIGHT)
						return true;
					return false;
				}
		}
		return true;
	}
}
