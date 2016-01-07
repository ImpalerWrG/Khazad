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
 * Extreamly simple Axis system used to build the rest of the Map system
 *
 * @author Impaler
 */
public enum Axis implements Serializable {

	AXIS_Z,	// Vertical
	AXIS_Y,	// North/South
	AXIS_X;	// East/West
}
