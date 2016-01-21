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
import java.io.Serializable;
/**
 * Consolidated all other coordinates under one object allowing full resolution
 * of any point in the game world and proper translation across all boundries
 * 
 * @author Impaler
 */
public class MapCoordinate implements Serializable  {

	private RegionCoordinate Region = null;
	public SectorCoordinate Sector = null;
	public ChunkCoordinate Chunk = null;
	public BlockCoordinate Block = null;
	
	public MapCoordinate() {
		this.Region = new RegionCoordinate();
		this.Sector = new SectorCoordinate();
		this.Chunk = new ChunkCoordinate();
		this.Block = new BlockCoordinate();
	}

	public MapCoordinate(ChunkCoordinate Chunk, BlockCoordinate Block) {
		this.Region = new RegionCoordinate();
		this.Sector = new SectorCoordinate();
		this.Chunk = Chunk.clone();
		this.Block = Block.clone();
	}

	public void setChunkCoordinate(ChunkCoordinate Chunk) {
		this.Chunk.copy(Chunk);
	}

	public void setBlockCoordinate(BlockCoordinate Block) {
		this.Block.copy(Block);
	}

	public void set(int x, int y, int z) {
		setX(x);
		setY(y);
		setZ(z);
	}

	public void setX(int x) {
		this.Chunk.X = (short) (x / BlockCoordinate.CHUNK_EDGE_SIZE);
		this.Block.set(Axis.AXIS_X, x % BlockCoordinate.CHUNK_EDGE_SIZE);
	}

	public void setY(int y) {
		this.Chunk.Y = (short) (y / BlockCoordinate.CHUNK_EDGE_SIZE);
		this.Block.set(Axis.AXIS_Y, y % BlockCoordinate.CHUNK_EDGE_SIZE);
	}

	public void setZ(int z) {
		this.Chunk.Z = (short) (z / BlockCoordinate.CHUNK_EDGE_SIZE);
		this.Block.set(Axis.AXIS_Z, z % BlockCoordinate.CHUNK_EDGE_SIZE);
	}

	public void translate(Direction DirectionType) {
		translateCube(DirectionType, 1);
	}

	public void translateCube(Direction DirectionType, int Quantity) {

		int Xtranslation = DirectionType.getValueonAxis(Axis.AXIS_X) * Quantity;
		int Ytranslation = DirectionType.getValueonAxis(Axis.AXIS_Y) * Quantity;
		int Ztranslation = DirectionType.getValueonAxis(Axis.AXIS_Z) * Quantity;

		int RawX = this.Block.getX() + Xtranslation;
		int RawY = this.Block.getY() + Ytranslation;
		int RawZ = this.Block.getZ() + Ztranslation;

		int CelltranslateX = RawX / BlockCoordinate.CHUNK_EDGE_SIZE;
		int CelltranslateY = RawY / BlockCoordinate.CHUNK_EDGE_SIZE;
		int CelltranslateZ = RawZ / BlockCoordinate.CHUNK_EDGE_SIZE;

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

		this.Chunk.X += CelltranslateX;
		this.Chunk.Y += CelltranslateY;
		this.Chunk.Z += CelltranslateZ;

		short CubeX = (short) (RawX % BlockCoordinate.CHUNK_EDGE_SIZE);
		short CubeY = (short) (RawY % BlockCoordinate.CHUNK_EDGE_SIZE);
		short CubeZ = (short) (RawZ % BlockCoordinate.CHUNK_EDGE_SIZE);

		this.Block.set(CubeX, CubeY, CubeZ);	
	}

	public Vector3f getVector() {
		return new Vector3f(getX(), getY(), getZ());
	}

	public int getX() {
		return (Chunk.X * BlockCoordinate.CHUNK_EDGE_SIZE) + Block.getX();
	}

	public int getY() {
		return (Chunk.Y * BlockCoordinate.CHUNK_EDGE_SIZE) + Block.getY();
	}

	public int getZ() {
		return (Chunk.Z * BlockCoordinate.CHUNK_EDGE_SIZE) + Block.getZ();
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
		return (Block.equals(Arg.Block) && Chunk.equals(Arg.Chunk) && Sector.equals(Arg.Sector) && Region.equals(Arg.Region));
	}

	@Override
	public MapCoordinate clone() {
		MapCoordinate newCoords = new MapCoordinate();
		newCoords.Block.copy(Block);
		newCoords.Chunk.copy(Chunk);
		newCoords.Sector.copy(Sector);
		newCoords.Region.copy(Region);
		return newCoords;
	}

	public void copy(MapCoordinate CopyCoordinates) {
		this.Block.copy(CopyCoordinates.Block);
		this.Chunk.copy(CopyCoordinates.Chunk);
		this.Sector.copy(CopyCoordinates.Sector);
		this.Region.copy(CopyCoordinates.Region);
	}

	@Override
	public int hashCode() {
		int hash = 3;
		hash += 17 * Chunk.X;
		hash += 37 * Chunk.Y;
		hash += 5 * Block.Data;
		return hash;
	}
}
