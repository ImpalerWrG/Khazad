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
 * Provides the location of a Sector within a Region, the world is only 1 Sector thick 
 * meaning a Sector has only an X and Y location coordinates within it's Region.
 * 
 * @author Impaler
 */
public class SectorCoordinate implements Serializable {

	byte X, Y;

	public SectorCoordinate() {
		X = 0;  Y = 0;
	}

	public SectorCoordinate(byte x, byte y) {
		X = x;  Y = y;
	}

	@Override
	public boolean equals(Object ArgumentCoordinates) {

		//if (ArgumentCoordinates == null)
		//return false;
		//if (ArgumentCoordinates == this)
		//return true;
		//if (!(ArgumentCoordinates instanceof MapCoordinate))
		//return false;

		SectorCoordinate Arg = (SectorCoordinate) ArgumentCoordinates;
		return (Arg.X == this.X && Arg.Y == this.Y);
	}

	public void copy(SectorCoordinate ArgumentCoordinates) {
		this.X = ArgumentCoordinates.X;
		this.Y = ArgumentCoordinates.Y;
	}

	@Override
	public int hashCode() {
		int hash = 3;
		hash += 17 * X;
		hash += 37 * Y;
		//hash += 5 * Z;
		return hash;
	}
}
