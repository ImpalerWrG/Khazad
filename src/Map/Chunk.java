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

import Data.DataManager;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.Serializable;

import java.util.HashMap;
import java.util.BitSet;

/**
 * Primary MapData storage class, holds all data for describing a set of blocks
 * and HashMaps hold face objects.
 *
 * @author Impaler
 */
public class Chunk implements Serializable {

	private static final long serialVersionUID = 1;
	// Larger DataValues specific to each Block
	private short[][][] BlockMaterialTypes;
	private short[][][] BlockShapeTypes;
	private short[][] SlabMaterialTypes;
	private short[][] SlabShapeTypes;
	private BitSet[] SlabMaterialInitialized;
	private BitSet[] SlabShapeInitialized;

	private BlockShape TestingBlockShape, AdjacentBlockShape;
	// Bit values for each Block
	private BitSet Hidden;
	private BitSet SubTerranean;
	private BitSet SkyView;
	private BitSet SunLit;
	// Keeps all Faces between and inside Blocks
	private HashMap<FaceCoordinate, Face>[] Faces;
	// The global position of this chunk relative to other chunks
	private ChunkCoordinate thisChunkCoordinates;
	private Sector parentSector;
	// Dirty values, set true on changes, set false by rendering
	transient boolean DirtyTerrainRendering;
	transient boolean DirtyPathRendering;

	protected static final short WallSurface = DataManager.getLabelIndex("SURFACETYPE_ROUGH_WALL");
	protected static final short FloorSurface = DataManager.getLabelIndex("SURFACETYPE_ROUGH_FLOOR_1");

	String ChunkString;

	public Chunk(Sector Parent) {
		parentSector = Parent;

		BlockMaterialTypes = new short[BlockCoordinate.CHUNK_DETAIL_LEVELS][][];
		BlockShapeTypes = new short[BlockCoordinate.CHUNK_DETAIL_LEVELS][][];
		SlabMaterialTypes = new short[BlockCoordinate.CHUNK_DETAIL_LEVELS][];
		SlabShapeTypes = new short[BlockCoordinate.CHUNK_DETAIL_LEVELS][];
		SlabMaterialInitialized = new BitSet[BlockCoordinate.CHUNK_DETAIL_LEVELS];
		SlabShapeInitialized = new BitSet[BlockCoordinate.CHUNK_DETAIL_LEVELS];

		Faces = (HashMap<FaceCoordinate, Face>[]) new HashMap<?, ?>[BlockCoordinate.CHUNK_DETAIL_LEVELS];

		for (int i = 0; i < BlockCoordinate.CHUNK_DETAIL_LEVELS; i++) {
			Faces[i] = new HashMap<FaceCoordinate, Face>();
			int Size = (BlockCoordinate.CHUNK_DETAIL_LEVELS - i) - 1;
			Size = 1 << Size;
			BlockShapeTypes[i] = new short[Size][];
			BlockMaterialTypes[i] = new short[Size][];
			SlabMaterialTypes[i] = new short[Size];
			SlabShapeTypes[i] = new short[Size];
			SlabMaterialInitialized[i] = new BitSet(Size);
			SlabShapeInitialized[i] = new BitSet(Size);

			for (int j = 0; j < Size; j++) {
				SlabMaterialTypes[i][j] = DataManager.INVALID_INDEX;
				SlabShapeTypes[i][j] = BlockShape.EMPTY_CUBE_DATA;
			}
		}

		Hidden = new BitSet(BlockCoordinate.BLOCKS_PER_CHUNK);
		SubTerranean = new BitSet(BlockCoordinate.BLOCKS_PER_CHUNK);
		SkyView = new BitSet(BlockCoordinate.BLOCKS_PER_CHUNK);
		SunLit = new BitSet(BlockCoordinate.BLOCKS_PER_CHUNK);

		TestingBlockShape = new BlockShape();
		AdjacentBlockShape = new BlockShape();
		DirtyTerrainRendering = true;
		DirtyPathRendering = true;
	}

	private void readObject(ObjectInputStream ois) throws ClassNotFoundException, IOException {
		// default deserialization
		ois.defaultReadObject();
		// fix transients
		DirtyTerrainRendering = true;
		DirtyPathRendering = true;
	}

	public void setChunkCoordinates(ChunkCoordinate Coordinates) {
		thisChunkCoordinates = Coordinates;
		ChunkString = getClass().getName() + thisChunkCoordinates.toString();
		setRenderingDirty();
	}

	public ChunkCoordinate getChunkCoordinates() {
		return thisChunkCoordinates.clone();
	}

	public void getBlockShape(BlockCoordinate Coordinates, BlockShape writeBlock) {
		short retrivedData;
		if (!SlabShapeInitialized[Coordinates.DetailLevel].get(Coordinates.getZ())) {
			retrivedData = SlabShapeTypes[Coordinates.DetailLevel][Coordinates.getZ()];
		} else {
			retrivedData = BlockShapeTypes[Coordinates.DetailLevel][Coordinates.getZ()][Coordinates.getXY()];	
		}
		writeBlock.setData(retrivedData);
	}

	public void setBlockShape(BlockCoordinate Coordinates, BlockShape NewShape) {
		if (NewShape.Data != SlabShapeTypes[Coordinates.DetailLevel][Coordinates.getZ()]) {

			if (!SlabShapeInitialized[Coordinates.DetailLevel].get(Coordinates.getZ())) {
				BlockShapeTypes[Coordinates.DetailLevel][Coordinates.getZ()] = new short[Coordinates.Size * Coordinates.Size];
				SlabShapeInitialized[Coordinates.DetailLevel].set(Coordinates.getZ());
				SlabShapeTypes[Coordinates.DetailLevel][Coordinates.getZ()] = BlockShape.EMPTY_CUBE_DATA;
			}

			if (NewShape.getData() != BlockShapeTypes[Coordinates.DetailLevel][Coordinates.getZ()][Coordinates.getXY()]) {
				BlockShapeTypes[Coordinates.DetailLevel][Coordinates.getZ()][Coordinates.getXY()] = NewShape.getData();

				Face TargetFace = getFace(new FaceCoordinate(Coordinates, Direction.DIRECTION_NONE));
				if (TargetFace != null) {
					setFaceShape(new FaceCoordinate(Coordinates, Direction.DIRECTION_NONE), new FaceShape(NewShape, null, Direction.DIRECTION_NONE));
				}
				setRenderingDirty();
			}
		}
	}

	public short getBlockMaterial(BlockCoordinate Coordinates) {
		if (!SlabMaterialInitialized[Coordinates.DetailLevel].get(Coordinates.getZ())) {
			return SlabMaterialTypes[Coordinates.DetailLevel][Coordinates.getZ()];
		} else {
			return BlockMaterialTypes[Coordinates.DetailLevel][Coordinates.getZ()][Coordinates.getXY()];	
		}
	}

	public void setBlockMaterial(BlockCoordinate Coordinates, short MaterialID) {		
		if (MaterialID != SlabMaterialTypes[Coordinates.DetailLevel][Coordinates.getZ()]) {

			if (!SlabMaterialInitialized[Coordinates.DetailLevel].get(Coordinates.getZ())) {
				BlockMaterialTypes[Coordinates.DetailLevel][Coordinates.getZ()] = new short[Coordinates.Size * Coordinates.Size];
				SlabMaterialInitialized[Coordinates.DetailLevel].set(Coordinates.getZ());
				SlabMaterialTypes[Coordinates.DetailLevel][Coordinates.getZ()] = DataManager.INVALID_INDEX;
			}

			if (MaterialID != BlockMaterialTypes[Coordinates.DetailLevel][Coordinates.getZ()][Coordinates.getXY()]) {
				BlockMaterialTypes[Coordinates.DetailLevel][Coordinates.getZ()][Coordinates.getXY()] = MaterialID;
			}
		}
	}

	public void buildFaces(int LevelofDetail) {
		GameMap ParentMap = GameMap.getMap();
		MapCoordinate AdjacentCoordinates = new MapCoordinate();
		MapCoordinate TargetCoordinates = new MapCoordinate();
	
		for (BlockCoordinate Index = new BlockCoordinate((byte) LevelofDetail); !Index.end(); Index.next()) {
			TestingBlockShape.Data = BlockShape.EMPTY_CUBE_DATA;
			getBlockShape(Index, TestingBlockShape);
			short BlockMaterial = getBlockMaterial(Index);

			TargetCoordinates.Chunk.copy(thisChunkCoordinates);
			TargetCoordinates.Block.copy(Index);

			for (Direction DirectionType : Direction.AXIAL_DIRECTIONS) {
				AdjacentCoordinates.setChunkCoordinate(thisChunkCoordinates);
				AdjacentCoordinates.setBlockCoordinate(Index);
				AdjacentCoordinates.translate(DirectionType);

				if (ParentMap.isBlockInitialized(AdjacentCoordinates)) {
					ParentMap.getBlockShape(AdjacentCoordinates, AdjacentBlockShape);

					if (AdjacentBlockShape.isSky()) {
						if (TestingBlockShape.hasFace(DirectionType)) {
							Face NewFace = ParentMap.addFace(TargetCoordinates, DirectionType);

							NewFace.setFaceMaterialType(BlockMaterial);
							NewFace.setFaceSurfaceType(WallSurface);
							NewFace.setFaceShapeType(new FaceShape(TestingBlockShape, AdjacentBlockShape, DirectionType));
							if (ParentMap.isBlockSunLit(AdjacentCoordinates))
								NewFace.Sunlit = true;
						}
					}

					if (!AdjacentBlockShape.isEmpty()) {
						if (DirectionType == Direction.DIRECTION_DOWN && TestingBlockShape.hasFloor() && AdjacentBlockShape.hasCeiling()) {
							Face NewFace = ParentMap.addFace(TargetCoordinates, DirectionType);

							NewFace.setFaceMaterialType(ParentMap.getBlockMaterial(AdjacentCoordinates));
							NewFace.setFaceSurfaceType(FloorSurface);
							NewFace.setFaceShapeType(new FaceShape(TestingBlockShape, AdjacentBlockShape, DirectionType));
							if (ParentMap.isBlockSunLit(AdjacentCoordinates))
								NewFace.Sunlit = true;
						}
					}
				}
			}

			if (!TestingBlockShape.isEmpty() && !TestingBlockShape.isSolid()) {
				Face NewFace = addFace(new FaceCoordinate(Index, Direction.DIRECTION_NONE));

				NewFace.setFaceMaterialType(BlockMaterial);
				NewFace.setFaceSurfaceType(FloorSurface);
				NewFace.setFaceShapeType(new FaceShape(TestingBlockShape, null, Direction.DIRECTION_NONE));
				if (isBlockSunLit(Index.getBlockIndex()))
					NewFace.Sunlit = true;
			}
		}
		setRenderingDirty();
	}

	public void growGrass() {
		DataManager Data = DataManager.getDataManager();

		for (Face TargetFace : Faces[0].values()) {
			if (TargetFace.Sunlit) {
				short MaterialID = TargetFace.getFaceMaterialType();
				int GrowthFactor = Data.getMaterialData(MaterialID).PlantGrowthFactor;
				if (GrowthFactor > 0) {
					TargetFace.setFaceMaterialType(Data.getLabelIndex("MATERIAL_DARK_GRASS"));
				}
			}
		}
	}

	public Face getFace(FaceCoordinate TargetCoordinates) {
		return Faces[TargetCoordinates.DetailLevel].get(TargetCoordinates);
	}

	boolean hasFace(FaceCoordinate TargetCoordinates) {
		return Faces[TargetCoordinates.DetailLevel].containsKey(TargetCoordinates);
	}

	short getFaceMaterialType(FaceCoordinate TargetCoordinates) {
		Face TargetFace = getFace(TargetCoordinates);
		return TargetFace != null ? TargetFace.getFaceMaterialType() : DataManager.INVALID_INDEX;
	}

	short getFaceSurfaceType(FaceCoordinate TargetCoordinates) {
		Face TargetFace = getFace(TargetCoordinates);
		return TargetFace != null ? TargetFace.getFaceSurfaceType() : DataManager.INVALID_INDEX;
	}

	boolean setFaceMaterialType(FaceCoordinate TargetCoordinates, short MaterialTypeID) {
		Face TargetFace = getFace(TargetCoordinates);

		if (TargetFace != null) {
			TargetFace.setFaceMaterialType(MaterialTypeID);
			setDirtyTerrainRendering(true);
			return true;
		}
		return false;
	}

	boolean setFaceSurfaceType(FaceCoordinate TargetCoordinates, short SurfaceTypeID) {
		Face TargetFace = getFace(TargetCoordinates);

		if (TargetFace != null) {
			TargetFace.setFaceSurfaceType(SurfaceTypeID);
			setDirtyTerrainRendering(true);
			return true;
		}
		return false;
	}

	FaceShape getFaceShape(FaceCoordinate TargetCoordinates) {
		Face TargetFace = getFace(TargetCoordinates);
		return TargetFace != null ? TargetFace.getFaceShapeType() : new FaceShape(new BlockShape(), new BlockShape(), Direction.DIRECTION_NONE);
	}

	boolean setFaceShape(FaceCoordinate TargetCoordinates, FaceShape NewShape) {
		Face TargetFace = getFace(TargetCoordinates);

		if (TargetFace != null) {
			TargetFace.setFaceShapeType(NewShape);
			setRenderingDirty();
			return true;
		}
		return false;
	}

	boolean removeFace(FaceCoordinate TargetCoordinates) {
		if (Faces[TargetCoordinates.DetailLevel].containsKey(TargetCoordinates)) {
			Faces[TargetCoordinates.DetailLevel].remove(TargetCoordinates);
			setRenderingDirty();
			return true;
		}
		return false;
	}

	Face addFace(FaceCoordinate TargetCoordinates) {
		int Detail = TargetCoordinates.DetailLevel;
		Face TargetFace = Faces[Detail].get(TargetCoordinates);
		if (TargetFace == null) {
			Face NewFace = new Face();
			Faces[Detail].put(TargetCoordinates, NewFace);
			setRenderingDirty();
			return NewFace;
		} else {
			return TargetFace;
		}
	}

	public HashMap<FaceCoordinate, Face> getFaces(int LevelofDetail) {
		return Faces[LevelofDetail];
	}

	public void setRenderingDirty() {
		setDirtyTerrainRendering(true);
		setDirtyPathingRendering(true);
	}

	public void setDirtyTerrainRendering(boolean dirty) {
		DirtyTerrainRendering = dirty;
	}

	public void setDirtyPathingRendering(boolean dirty) {
		DirtyPathRendering = dirty;
	}

	public boolean isTerrainRenderingDirty() {
		return DirtyTerrainRendering;
	}

	public boolean isPathingRenderingDirty() {
		return DirtyPathRendering;
	}

	public boolean isBlockHidden(short Coordinates) {
		return Hidden.get(Coordinates);
	}

	public void setBlockHidden(short Coordinates, boolean NewValue) {
		Hidden.set(Coordinates, NewValue);
		DirtyTerrainRendering = true;
	}

	public boolean isBlockSubTerranean(short Coordinates) {
		return SubTerranean.get(Coordinates);
	}

	public void setBlockSubTerranean(short Coordinates, boolean NewValue) {
		SubTerranean.set(Coordinates, NewValue);
		DirtyTerrainRendering = true;
	}

	public boolean isBlockSkyView(short Coordinates) {
		return SkyView.get(Coordinates);
	}

	public void setBlockSkyView(short Coordinates, boolean NewValue) {
		SkyView.set(Coordinates, NewValue);
		DirtyTerrainRendering = true;
	}

	public boolean isBlockSunLit(short Coordinates) {
		return SunLit.get(Coordinates);
	}

	public void setBlockSunLit(short Coordinates, boolean NewValue) {
		SunLit.set(Coordinates, NewValue);
		DirtyTerrainRendering = true;
	}

	@Override
	public String toString() {
		return ChunkString;
	}
}
