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

package PathFinding;

import java.io.Serializable;

/**
 * Encompases all the unique properties of a Pawns mode of movement that
 * may be relevent for pathfinding. MovementType specifies the kind of surface
 * or material a Pawn can move through such as Air, water etc. Size reflects
 * the sides of the cubic volume it must have open, a size 2 creature needs a 2x2x2
 * volume to move through. Speed determines relative edge weights. A modality
 * can have different sizes for different modes, such as size 2 when flying and
 * size 1 when walking.
 *
 * @author Impaler
 */
public class MovementModality implements Serializable {

	private static final long serialVersionUID = 1;

	public enum MovementType {

		MOVEMENT_TYPE_WALK,
		MOVEMENT_TYPE_WHEEL,
		MOVEMENT_TYPE_CLIMB,
		MOVEMENT_TYPE_SWIM,
		MOVEMENT_TYPE_FLY
	}

	public boolean[] MovementTypeCapable;
	public int[] MovementTypeSize;
	public float[] MovementTypeSpeed;

	public MovementModality(MovementType MovementType, float Speed, int Size) {
		MovementTypeCapable = new boolean[MovementType.values().length];
		MovementTypeSize = new int[MovementType.values().length];
		MovementTypeSpeed = new float[MovementType.values().length];

		MovementTypeCapable[MovementType.ordinal()] = true;
		MovementTypeSize[MovementType.ordinal()] = Size;
		MovementTypeSpeed[MovementType.ordinal()] = Speed;
	}

	public void setModalitySpeed(MovementType MovementType, float Speed) {
		MovementTypeSpeed[MovementType.ordinal()] = Speed;
	}

	public void setModalitySize(MovementType MovementType, int Size) {
		MovementTypeSpeed[MovementType.ordinal()] = Size;
	}

	@Override
	public boolean equals(Object Argument) {
		if (Argument == null)
			return false;
		if (Argument == this)
			return true;
		if (!(Argument instanceof MovementModality))
			return false;

		MovementModality Arg = (MovementModality) Argument;
		boolean CapableEquality = true;
		for (MovementType Type : MovementType.values()) {
			if (Arg.MovementTypeCapable[Type.ordinal()] != this.MovementTypeCapable[Type.ordinal()]) {
				CapableEquality = false;
				break;
			}
		}

		boolean SizeEquality = true;
		for (MovementType Type : MovementType.values()) {
			if (Arg.MovementTypeSize[Type.ordinal()] != this.MovementTypeSize[Type.ordinal()]) {
				SizeEquality = false;
				break;
			}
		}

		return CapableEquality && SizeEquality;
	}

	@Override
	public int hashCode() {
		int hash = 3;


		//hash += 17 * MovementTypeCapable.hashCode();
		//hash += 37 * MovementTypeSize.hashCode();
		//hash += 5 * MovementTypeSpeed.hashCode();
		return hash;
	}
}
