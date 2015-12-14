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

import Data.DataManager;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.Serializable;

import java.util.HashMap;
import java.util.BitSet;

import org.javatuples.Pair;

/**
 * Primary MapData storage class, holds all data for describing a 16x16 square
 * 1 level high region of the map, each tile is indexed by a byte for array and
 * bitset based data and HashMaps hold face objects.
 *
 * @author Impaler
 */
public class Cell implements Serializable {

	private static final long serialVersionUID = 1;
	// Larger DataValues specific to each Cube
	private short[] CubeMaterialTypes;
	private short[][] CubeShapeTypes;
	// Bit values for each Cube
	private BitSet Hidden;
	private BitSet SubTerranean;
	private BitSet SkyView;
	private BitSet SunLit;
	// Keeps all Faces between and inside Cubes
	private HashMap<FaceCoordinate, Face>[] Faces;
	// The global position of this cell relative to other cells
	private CellCoordinate thisCellCoordinates;
	// Dirty values, set true on changes, set false by rendering
	transient boolean DirtyTerrainRendering;
	transient boolean DirtyPathRendering;

	protected static final short WallSurface = DataManager.getLabelIndex("SURFACETYPE_ROUGH_WALL");
	protected static final short FloorSurface = DataManager.getLabelIndex("SURFACETYPE_ROUGH_FLOOR_1");

	String CellString;

	public Cell() {
		CubeMaterialTypes = new short[CubeCoordinate.CUBESPERCELL];
		CubeShapeTypes = new short[CubeCoordinate.CELLDETAILLEVELS][];
		Faces = (HashMap<FaceCoordinate, Face>[]) new HashMap<?, ?>[CubeCoordinate.CELLDETAILLEVELS];

		for (int i = 0; i < CubeCoordinate.CELLDETAILLEVELS; i++) {
			Faces[i] = new HashMap<FaceCoordinate, Face>();
			int Size = (CubeCoordinate.CELLDETAILLEVELS - i) - 1;
			Size = 1 << Size;
			CubeShapeTypes[i] = new short[Size * Size];
			for (int j = 0; j < CubeShapeTypes[i].length; j++) {
				CubeShapeTypes[i][j] = CubeShape.EMPTY_CUBE_DATA;
			}
		}

		Hidden = new BitSet(CubeCoordinate.CUBESPERCELL);
		SubTerranean = new BitSet(CubeCoordinate.CUBESPERCELL);
		SkyView = new BitSet(CubeCoordinate.CUBESPERCELL);
		SunLit = new BitSet(CubeCoordinate.CUBESPERCELL);

		for (int i = 0; i < CubeMaterialTypes.length; i++) {
			CubeMaterialTypes[i] = DataManager.INVALID_INDEX;
		}

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

	public void setCellCoordinates(CellCoordinate Coordinates) {
		thisCellCoordinates = Coordinates;
		CellString = getClass().getName() + thisCellCoordinates.toString();
		setRenderingDirty();
	}

	public CellCoordinate getCellCoordinates() {
		return thisCellCoordinates.clone();
	}

	public void setCubeShape(short Coordinates, CubeShape NewShape, int Level) {
		if (NewShape.getData() != CubeShapeTypes[Level][Coordinates]) {
			CubeShapeTypes[Level][Coordinates] = NewShape.getData();

			Face TargetFace = getFace(new FaceCoordinate(Coordinates, Direction.DIRECTION_NONE), 0);
			if (TargetFace != null) {
				setFaceShape(new FaceCoordinate(Coordinates, Direction.DIRECTION_NONE), new FaceShape(NewShape, null, Direction.DIRECTION_NONE));
			}
			setRenderingDirty();
		}
	}

	public void setCubeShape(short Coordinates, CubeShape NewShape) {
		if (NewShape.getData() != CubeShapeTypes[0][Coordinates]) {
			CubeShapeTypes[0][Coordinates] = NewShape.getData();

			Face TargetFace = getFace(new FaceCoordinate(Coordinates, Direction.DIRECTION_NONE), 0);
			if (TargetFace != null) {
				setFaceShape(new FaceCoordinate(Coordinates, Direction.DIRECTION_NONE), new FaceShape(NewShape, null, Direction.DIRECTION_NONE));
			}
			setRenderingDirty();
		}	
	}
	
	public void buildFaces(int LevelofDetail) {
		GameMap ParentMap = GameMap.getMap();
	
		int SizeFactor = (CubeCoordinate.CELLDETAILLEVELS - LevelofDetail) - 1;
		int Size = 1 << SizeFactor;
		int Count = Size * Size;
		for (short j = 0; j < Count; j++) {
			CubeShape Shape = getCubeShape(j, LevelofDetail);
			short CubeMaterial = getCubeMaterial(j);

			for (Direction DirectionType : Direction.AXIAL_DIRECTIONS) {
				Pair <CellCoordinate, Short> values = ParentMap.TranslateCubeIndex(thisCellCoordinates, j, DirectionType, LevelofDetail);				
				CubeCoordinate AdjacentCoordinates = new CubeCoordinate(values.getValue0(), values.getValue1().shortValue());

				if (ParentMap.isCubeInited(AdjacentCoordinates)) {
					CubeShape AdjacentShape = ParentMap.getCubeShape(AdjacentCoordinates);

					if (AdjacentShape.isSky()) {
						if (Shape.hasFace(DirectionType)) {
							Face NewFace = ParentMap.addFace(new CubeCoordinate(thisCellCoordinates, j), DirectionType, LevelofDetail);

							NewFace.setFaceMaterialType(CubeMaterial);
							NewFace.setFaceSurfaceType(WallSurface);
							NewFace.setFaceShapeType(new FaceShape(Shape, AdjacentShape, DirectionType));
							if (ParentMap.isCubeSunLit(AdjacentCoordinates))
								NewFace.Sunlit = true;
						}
					}

					if (!AdjacentShape.isEmpty()) {
						if (DirectionType == Direction.DIRECTION_DOWN && Shape.hasFloor() && AdjacentShape.hasCeiling()) {
							Face NewFace = ParentMap.addFace(new CubeCoordinate(thisCellCoordinates, j), DirectionType, LevelofDetail);

							NewFace.setFaceMaterialType(ParentMap.getCubeMaterial(AdjacentCoordinates));
							NewFace.setFaceSurfaceType(FloorSurface);
							NewFace.setFaceShapeType(new FaceShape(Shape, AdjacentShape, DirectionType));
							if (ParentMap.isCubeSunLit(AdjacentCoordinates))
								NewFace.Sunlit = true;
						}
					}
				}
			}

			if (!Shape.isEmpty() && !Shape.isSolid()) {
				Face NewFace = addFace(new FaceCoordinate(j, Direction.DIRECTION_NONE, LevelofDetail), LevelofDetail);

				NewFace.setFaceMaterialType(CubeMaterial);
				NewFace.setFaceSurfaceType(FloorSurface);
				NewFace.setFaceShapeType(new FaceShape(Shape, null, Direction.DIRECTION_NONE));
				if (isCubeSunLit(j))
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

	public Face getFace(FaceCoordinate TargetCoordinates, int LevelofDetail) {
		return Faces[LevelofDetail].get(TargetCoordinates);
	}

	boolean hasFace(FaceCoordinate TargetCoordinates, int LevelofDetail) {
		return Faces[LevelofDetail].containsKey(TargetCoordinates);
	}

	short getFaceMaterialType(FaceCoordinate TargetCoordinates) {
		Face TargetFace = getFace(TargetCoordinates, 0);
		return TargetFace != null ? TargetFace.getFaceMaterialType() : DataManager.INVALID_INDEX;
	}

	short getFaceSurfaceType(FaceCoordinate TargetCoordinates) {
		Face TargetFace = getFace(TargetCoordinates, 0);
		return TargetFace != null ? TargetFace.getFaceSurfaceType() : DataManager.INVALID_INDEX;
	}

	boolean setFaceMaterialType(FaceCoordinate TargetCoordinates, short MaterialTypeID) {
		Face TargetFace = getFace(TargetCoordinates, 0);

		if (TargetFace != null) {
			TargetFace.setFaceMaterialType(MaterialTypeID);
			setDirtyTerrainRendering(true);
			return true;
		}
		return false;
	}

	boolean setFaceSurfaceType(FaceCoordinate TargetCoordinates, short SurfaceTypeID) {
		Face TargetFace = getFace(TargetCoordinates, 0);

		if (TargetFace != null) {
			TargetFace.setFaceSurfaceType(SurfaceTypeID);
			setDirtyTerrainRendering(true);
			return true;
		}
		return false;
	}

	FaceShape getFaceShape(FaceCoordinate TargetCoordinates) {
		Face TargetFace = getFace(TargetCoordinates, 0);
		return TargetFace != null ? TargetFace.getFaceShapeType() : new FaceShape(new CubeShape(), new CubeShape(), Direction.DIRECTION_NONE);
	}

	boolean setFaceShape(FaceCoordinate TargetCoordinates, FaceShape NewShape) {
		Face TargetFace = getFace(TargetCoordinates, 0);

		if (TargetFace != null) {
			TargetFace.setFaceShapeType(NewShape);
			setRenderingDirty();
			return true;
		}
		return false;
	}

	boolean removeFace(FaceCoordinate TargetCoordinates, int LevelofDetail) {
		if (Faces[LevelofDetail].containsKey(TargetCoordinates)) {
			Faces[LevelofDetail].remove(TargetCoordinates);
			setRenderingDirty();
			return true;
		}
		return false;
	}

	Face addFace(FaceCoordinate TargetCoordinates, int LevelofDetail) {
		Face TargetFace = Faces[LevelofDetail].get(TargetCoordinates);
		if (TargetFace == null) {
			Face NewFace = new Face(TargetCoordinates.getCoordinates(), TargetCoordinates.getFaceDirection());
			Faces[LevelofDetail].put(TargetCoordinates, NewFace);
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

	public CubeShape getCubeShape(short Index, int LevelofDetal) {
		return new CubeShape(CubeShapeTypes[LevelofDetal][Index]);
	}

	public short getCubeMaterial(short Coordinates) {
		return CubeMaterialTypes[Coordinates];
	}

	public void setCubeMaterial(short Coordinates, short MaterialID) {
		CubeMaterialTypes[Coordinates] = MaterialID;
		DirtyTerrainRendering = true;
	}

	public boolean isCubeHidden(short Coordinates) {
		return Hidden.get(Coordinates);
	}

	public void setCubeHidden(short Coordinates, boolean NewValue) {
		Hidden.set(Coordinates, NewValue);
		DirtyTerrainRendering = true;
	}

	public boolean isCubeSubTerranean(short Coordinates) {
		return SubTerranean.get(Coordinates);
	}

	public void setCubeSubTerranean(short Coordinates, boolean NewValue) {
		SubTerranean.set(Coordinates, NewValue);
		DirtyTerrainRendering = true;
	}

	public boolean isCubeSkyView(short Coordinates) {
		return SkyView.get(Coordinates);
	}

	public void setCubeSkyView(short Coordinates, boolean NewValue) {
		SkyView.set(Coordinates, NewValue);
		DirtyTerrainRendering = true;
	}

	public boolean isCubeSunLit(short Coordinates) {
		return SunLit.get(Coordinates);
	}

	public void setCubeSunLit(short Coordinates, boolean NewValue) {
		SunLit.set(Coordinates, NewValue);
		DirtyTerrainRendering = true;
	}

	@Override
	public String toString() {
		return CellString;
	}
}
