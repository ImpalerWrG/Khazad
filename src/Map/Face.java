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

import Map.Coordinates.Direction;
import Data.DataManager;
import java.io.Serializable;

/**
 * Describes a Face both its shape, location inside a Chunk and it's material
 * for gameplay purposes.
 *
 * @author Impaler
 */
public class Face implements Serializable {

	private static final long serialVersionUID = 1;
	// Shape and location Index
	private FaceShape FaceType;
	//byte LocationCoordinates;
	boolean Sunlit;
	// DataType Indexes
	private short SurfaceTypeID;
	private short MaterialTypeID;

	public Face() {
		SurfaceTypeID = DataManager.INVALID_INDEX;
		MaterialTypeID = DataManager.INVALID_INDEX;

		BlockShape SourceBlock = new BlockShape((byte) 0);
		BlockShape AdjacentBlock = new BlockShape((byte) 0);
		FaceType = new FaceShape(SourceBlock, AdjacentBlock, Direction.DIRECTION_NONE);
	}

	public Face(byte TargetCoordinates, Direction DirectionType) {
		SurfaceTypeID = DataManager.INVALID_INDEX;
		MaterialTypeID = DataManager.INVALID_INDEX;

		BlockShape SourceBlock = new BlockShape((byte) 0);
		BlockShape AdjacentBlock = new BlockShape((byte) 0);
		FaceType = new FaceShape(SourceBlock, AdjacentBlock, Direction.DIRECTION_NONE);
		//LocationCoordinates = TargetCoordinates;
	}

	public void setFaceMaterialType(short NewMaterialTypeID) {
		if (NewMaterialTypeID != MaterialTypeID) {
			MaterialTypeID = NewMaterialTypeID;
			//Set Chunk needsRedraw
			if (MaterialTypeID == -1) {
				int x = 0;
			}
		}
	}

	public void setFaceSurfaceType(short NewSurfaceTypeID) {
		if (NewSurfaceTypeID != SurfaceTypeID) {
			SurfaceTypeID = NewSurfaceTypeID;
			//Set Chunk needsRedraw
		}
	}

	public void setFaceShapeType(FaceShape NewShape) {
		if (NewShape != FaceType) {
			FaceType = NewShape;
			//Set Chunk needsRedraw
		}
	}

	public short getFaceMaterialType() {
		return MaterialTypeID;
	}

	public short getFaceSurfaceType() {
		return SurfaceTypeID;
	}

	public FaceShape getFaceShapeType() {
		return FaceType;
	}

	public boolean isSunlit() {
		return this.Sunlit;
	}
}
