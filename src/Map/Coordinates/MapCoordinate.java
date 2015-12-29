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

import com.jme3.math.Vector3f;
/**
 * Consolidated all other coordinates under one object allowing full resolution
 * of any point in the game world and proper translation across all boundries
 * 
 * @author Impaler
 */
public class MapCoordinate {

	//public RegionCoordinate Region = null;
	//public SectionCoordinate Section = null;
	public CellCoordinate Cell = null; // Rename to Chunk
	public CubeIndex Cube = null;    // Rename to Block
	
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

	public void set(int x, int y, int z) {

		this.Cell.X = (short) (x / CubeCoordinate.CELLEDGESIZE);
		if (x < 0) {
			this.Cell.X--;
			x += (CubeCoordinate.CELLEDGESIZE * this.Cell.X);
		}
		this.Cube.set(Axis.AXIS_X, x % CubeCoordinate.CELLEDGESIZE);
		
		this.Cell.Y = (short) (y / CubeCoordinate.CELLEDGESIZE);
		if (y < 0) {
			this.Cell.Y--;
			y += (CubeCoordinate.CELLEDGESIZE * this.Cell.Y);
		}
		this.Cube.set(Axis.AXIS_Y, y % CubeCoordinate.CELLEDGESIZE);
		
		this.Cell.Z = (short) (z / CubeCoordinate.CELLEDGESIZE);
		if (z < 0) {
			this.Cell.Z--;
			z += (CubeCoordinate.CELLEDGESIZE * this.Cell.Z);			
		}
		this.Cube.set(Axis.AXIS_Z, z % CubeCoordinate.CELLEDGESIZE);
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

	public void copy(MapCoordinate CopyCoordinates) {
		this.Cell.copy(CopyCoordinates.Cell);
		this.Cube.copy(CopyCoordinates.Cube);
	}

	public Vector3f getVector() {
		return new Vector3f(getX(), getY(), getZ());
	}

	public int getX() {
		return (Cell.X * CubeCoordinate.CELLEDGESIZE) + Cube.getX();
	}

	public int getY() {
		return (Cell.Y * CubeCoordinate.CELLEDGESIZE) + Cube.getY();
	}

	public int getZ() {
		return (Cell.Z * CubeCoordinate.CELLEDGESIZE) + Cube.getZ();
	}

	@Override
	public boolean equals(Object ArgumentCoordinates) {

		//if (ArgumentCoordinates == null)
		//return false;
		//if (ArgumentCoordinates == this)
		//return true;
		//if (!(ArgumentCoordinates instanceof MapCoordinate))
		//return false;

		MapCoordinate Arg = (MapCoordinate) ArgumentCoordinates;
		return (Cell.equals(Arg.Cell) && Cube.Data == Arg.Cube.Data && Cube.DetailLevel == Arg.Cube.DetailLevel);
	}

	@Override
	public MapCoordinate clone() {
		return new MapCoordinate(this.Cell, this.Cube);
	}

	@Override
	public int hashCode() {
		int hash = 3;
		hash += 17 * Cell.X;
		hash += 37 * Cell.Y;
		hash += 5 * Cube.Data;
		return hash;
	}
}
