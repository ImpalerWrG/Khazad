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
 * Used to determine the Mesh used to build a Rendering of a Cell
 *
 * @author Impaler
 */
public class FaceShape implements Serializable {

	private static final long serialVersionUID = 1;
	// TODO store as shorts, not objects as in Cell
	private CubeShape SourceCubeComponent;
	private CubeShape AdjacentCubeComponent;
	private byte FaceDirection;

	public FaceShape() {
		SourceCubeComponent = new CubeShape(CubeShape.BELOW_CUBE_HEIGHT);
		AdjacentCubeComponent = new CubeShape(CubeShape.BELOW_CUBE_HEIGHT);
		FaceDirection = (byte) Direction.DIRECTION_DESTINATION.ordinal();
	}

	public FaceShape(CubeShape SourceShapeType, CubeShape AdjacentShapeType, Direction DirectionType) {
		SourceCubeComponent = SourceShapeType;
		AdjacentCubeComponent = AdjacentShapeType;
		FaceDirection = (byte) DirectionType.ordinal();		
	}

	boolean equals(FaceShape ArgumentShape) {
		boolean SourceEqual = SourceCubeComponent.equals(ArgumentShape.SourceCubeComponent);
		boolean AdjacentEqual = false;
		if (AdjacentCubeComponent == null) {
			if (ArgumentShape.AdjacentCubeComponent == null)
				AdjacentEqual = true;
		} else {
			AdjacentEqual = AdjacentCubeComponent.equals(ArgumentShape.AdjacentCubeComponent);
		}
		return SourceEqual && AdjacentEqual && FaceDirection == ArgumentShape.FaceDirection;
	}

	boolean notequal(FaceShape ArgumentShape) {
		return !SourceCubeComponent.equals(ArgumentShape.SourceCubeComponent) || !AdjacentCubeComponent.equals(ArgumentShape.AdjacentCubeComponent) || FaceDirection != ArgumentShape.FaceDirection;
	}

	public Direction getFaceDirection() {
		return Direction.ANGULAR_DIRECTIONS[FaceDirection];
	}
	
	public CubeShape getSourceCubeShape() {
		return SourceCubeComponent;
	}

	public CubeShape getAdjacentCubeShape() {
		return AdjacentCubeComponent;
	}
}
