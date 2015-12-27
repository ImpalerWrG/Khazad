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


/**
 * Consolidated all other coordinates under one object allowing full resolution
 * of any point in the game world and proper translation across all boundries
 * 
 * @author Impaler
 */
public class MapCoordinate {

	// Region Coordinate Region;
	// Block Coordinate Block;
	public CellCoordinate Cell = null;
	public CubeIndex Cube = null;
	
	public MapCoordinate() {
		this.Cell = new CellCoordinate();
		this.Cube = new CubeIndex();
	}

	public MapCoordinate(CellCoordinate Cell, CubeIndex Cube) {
		this.Cell = Cell.clone();
		this.Cube = Cube.clone();
	}

	public void setCellCoordinate(CellCoordinate Cell) {
		this.Cell.copy(Cell);
	}

	public void setCubeCoordinate(CubeIndex Cube) {
		this.Cube.copy(Cube);
	}
	
	public void translate(Direction DirectionType) {
		translateCube(DirectionType, 1);
	}

	public void translateCube(Direction DirectionType, int Quantity) {

		int Xtranslation = DirectionType.getValueonAxis(Axis.AXIS_X) * Quantity;
		int Ytranslation = DirectionType.getValueonAxis(Axis.AXIS_Y) * Quantity;
		int Ztranslation = DirectionType.getValueonAxis(Axis.AXIS_Z) * Quantity;

		int RawX = this.Cube.getX() + Xtranslation;
		int RawY = this.Cube.getY() + Ytranslation;
		int RawZ = this.Cube.getZ() + Ztranslation;

		int CelltranslateX = RawX / CubeCoordinate.CELLEDGESIZE;
		int CelltranslateY = RawY / CubeCoordinate.CELLEDGESIZE;
		int CelltranslateZ = RawZ / CubeCoordinate.CELLEDGESIZE;

		if (RawX < 0) {
			//RawX += CubeCoordinate.CELLEDGESIZE;
			CelltranslateX += -1;
		}
		if (RawY < 0) {
			//RawY += CubeCoordinate.CELLEDGESIZE;
			CelltranslateY += -1;
		}
		if (RawZ < 0) {
			//RawZ += CubeCoordinate.CELLEDGESIZE;
			CelltranslateZ += -1;
		}

		this.Cell.X += CelltranslateX;
		this.Cell.Y += CelltranslateY;
		this.Cell.Z += CelltranslateZ;

		short CubeX = (short) (RawX % CubeCoordinate.CELLEDGESIZE);
		short CubeY = (short) (RawY % CubeCoordinate.CELLEDGESIZE);
		short CubeZ = (short) (RawZ % CubeCoordinate.CELLEDGESIZE);

		this.Cube.set(CubeX, CubeY, CubeZ);	
	}
}
