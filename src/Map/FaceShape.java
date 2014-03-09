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
 * Used to determine the Mesh used to build a Rendering of a Cell
 * @author Impaler
 */
public class FaceShape {

    CubeShape CubeComponent;
    Direction FaceDirection;
	
    public FaceShape() {
        CubeComponent = new CubeShape(CubeShape.BELOW_CUBE_HEIGHT);
        FaceDirection = Direction.DIRECTION_DESTINATION;
    }

    public FaceShape(CubeShape ShapeType, Direction DirectionType) {
        CubeComponent = ShapeType;
        FaceDirection = DirectionType;
    }

    boolean equals(FaceShape ArgumentShape) {
        return CubeComponent == ArgumentShape.CubeComponent && FaceDirection == ArgumentShape.FaceDirection;
    }

    boolean notequal(FaceShape ArgumentShape) {
        return CubeComponent != ArgumentShape.CubeComponent || FaceDirection != ArgumentShape.FaceDirection;
    }
}
